#include "DBMangerClient.h"
#include "MySQLite3.h"
#include "TinyXML/XmlIniFile.h"
#include "TinyXML/tinyxml.h"
#include "TinyXML/XmlCom.h"
#include "DbStdafx.h"
#include "StdAfx.h"
#include "HostInfo.h"
#include <time.h>

CDBMangerClient::CDBMangerClient() :
	m_dbHandle(new CMySQLite3),
	m_IniHandle(new CXmlIniFile(CONFIG_PATH)),
	m_MutexLock(new CommonMutex),
	isDownload(1),
	reqTime(0),
	iNetType(1)
{
}

CDBMangerClient::~CDBMangerClient()
{
	delete m_MutexLock;
	delete m_IniHandle;
	delete m_dbHandle;
}

int CDBMangerClient::Init(int CorS)
{
	CXmlIniFile iniFile(CONFIG_SOFT_TYPE);
	if(iniFile.GetKeyCount("NetType"))
	{
		iniFile.ReadInt("NetType", iNetType);
	}

	if(1 != iNetType)
	{
		OutPutStr("Soft is alone type\n");
		return 0;
	}

	if(!m_dbHandle->Open(DB_NAME))
	{
		OutPutStr(".....................Open db %s failed\n", DB_NAME);
	}

	GetHostIpAddr(strHostIp);
	GetControlledHostID(strHostId);

	//int isDownload = 1;
	if(m_IniHandle->GetKeyCount("IsDownload") > 0)
	{
		m_IniHandle->ReadInt("IsDownload", isDownload);
		if(0 == isDownload)
		{
			//vector<string> vecTables;
			int count = m_IniHandle->GetKeyCount("DownloadTabList", "Table");
			for(int i = 0; i < count; ++i)
			{
				string table;
				m_IniHandle->ReadStr("DownloadTabList", "Table", table, i);
				m_dbHandle->ClearTable(table);
				vecDownloadTabs.push_back(table);
			}
			//SendReq(REQ_DOWN, vecTables);
			//m_IniHandle->WriteInt("IsDownload", 1);
			//m_IniHandle->Save();
		}
	}

	int Upload = 1;
	if(m_IniHandle->GetKeyCount("IsUpload") > 0)
	{
		m_IniHandle->ReadInt("IsUpload", Upload);
		if(0 == Upload)
		{
			vector<string> vecTables;
			int count = m_IniHandle->GetKeyCount("UploadTabList", "Table");
			string table;
			for(int i = 0; i < count; ++i)
			{
				m_IniHandle->ReadStr("UploadTabList", "Table", table, i);
				vecTables.push_back(table);
			}
			SendReq(REQ_UP, vecTables);
			//m_IniHandle->WriteInt("IsUpload", 1);
			//m_IniHandle->Save();
		}
	}
	
	RunThread();
	return 0;
}

int CDBMangerClient::UnInit()
{
	StopThread();
	ClearQueue();
	return 0;
}

int CDBMangerClient::DoOnce()
{
	return 0;
}

int CDBMangerClient::SetMessage(struct struI_CommonMSG * p)
{
	if(1 != iNetType)
	{
		return 0;
	}

	if(IsNULL(p))
	{
		return 0;
	}

	if(CS_CLIENT == p->iDesType &&
	   TSM_PROID == p->iDesProductID&&
	   TSM_DBMID == p->iDesBussinessID)
	{
		UseCommonMutex(m_MutexLock);
		m_MsgQueue.push(CopyCommonMSGEx(p));
	}

	return 0;
}

BOOL CDBMangerClient::Work(void)
{
	if(0 == isDownload && time(NULL) - reqTime > 10)
	{
		SendReq(REQ_DOWN, vecDownloadTabs);
		reqTime = time(NULL);
	}

	UseCommonMutex(m_MutexLock);
	while(!m_MsgQueue.empty())
	{
		struI_CommonMSG* pMSG = m_MsgQueue.front();
		HandleResponse(pMSG);
		m_MsgQueue.pop();
		FreeCommonMSG(pMSG);
	}
	Sleep(1);
	return TRUE;
}

void CDBMangerClient::ClearQueue()
{
	UseCommonMutex(m_MutexLock);
	while(!m_MsgQueue.empty())
	{
		struI_CommonMSG* pMSG =  m_MsgQueue.front();
		m_MsgQueue.pop();
		FreeCommonMSG(pMSG);
	}
}

void CDBMangerClient::SendReq(int reqType, const vector<std::string>& vecTables)
{
	TiXmlDocument doc;
	TiXmlElement* pReqRoot = new TiXmlElement("Requst");
	TiXmlElement* pHostIDElem = new TiXmlElement("HostID");
	TiXmlElement* pHostIPElem = new TiXmlElement("HostIP");
	TiXmlElement* pTimeElem = new TiXmlElement("Time");

	CXmlCom::SetText(pHostIDElem, strHostId);
	CXmlCom::SetText(pHostIPElem, strHostIp);

	CXmlCom::AddChildNode(&doc, pReqRoot);
	CXmlCom::AddChildNode(pReqRoot, pHostIDElem);
	CXmlCom::AddChildNode(pReqRoot, pHostIPElem);
	CXmlCom::AddChildNode(pReqRoot, pTimeElem);

	TiXmlElement* pItemList(NULL);
	if(REQ_UP == reqType)
	{
		CXmlCom::SetAttribtue(pReqRoot, "type", "upload");
		pItemList = new TiXmlElement("NoResultSQL");
		CXmlCom::AddChildNode(pReqRoot, pItemList);
		for(int i = 0; i < vecTables.size(); ++i)
		{
			int Offset = 0;
			int Count = (m_dbHandle->GetRowNum(vecTables[i]) + ROW_NUM_MAX - 1) / ROW_NUM_MAX;
			for(int k = 0; k < Count; k++, Offset += ROW_NUM_MAX)
			{
				std::string strSQL;
				pItemList->Clear();
				if(-1 == m_dbHandle->Export(vecTables[i], strSQL, Offset, ROW_NUM_MAX))
				{
					OutPutStr("Export failed tableName = %s, Offset = %d, Num = %d\n", 
						vecTables[i].c_str(),
						Offset,
						ROW_NUM_MAX);
				}

				TiXmlElement* pSQLElem =  new TiXmlElement("SQL");
				CXmlCom::SetText(pSQLElem, strSQL);
				CXmlCom::AddChildNode(pItemList, pSQLElem);
				std::string strXml = CXmlCom::ToString(&doc);
				OutPutStr("strReq = %s\n", strXml.c_str());
				Send2Server(strXml);
			}	
		}

	}
	else if(REQ_DOWN == reqType)
	{
		CXmlCom::SetAttribtue(pReqRoot, "type", "download");
		pItemList = new TiXmlElement("TableList");
		for(int i = 0; i < vecTables.size(); ++i)
		{
			TiXmlElement* pTabElem =  new TiXmlElement("Table");
			CXmlCom::SetText(pTabElem, vecTables[i]);
			CXmlCom::AddChildNode(pItemList, pTabElem);
		}
		CXmlCom::AddChildNode(pReqRoot, pItemList);
		std::string strXml = CXmlCom::ToString(&doc);
		OutPutStr("strReq = %s\n", strXml.c_str());
		Send2Server(strXml);
	}
}

void CDBMangerClient::Send2Server(std::string& strXml)
{
	struI_CommonMSG* p = CreateCommonMSG((BYTE*)strXml.c_str(), 
					strXml.size(),
					CS_SERVER,
					TSM_PROID,
					TSM_DBMID,
					CS_CLIENT,
					TSM_PROID,
					TSM_DBMID);
	if(m_pSendMFunc)
	{
		m_pSendMFunc(p);
		OutPutStr("Send Ok!\n");
	}
	else
	{
		FreeCommonMSG(p);
		OutPutStr("Send failed!\n");
	}
}

void CDBMangerClient::HandleResponse(struI_CommonMSG* pMSG)
{
	OutPutStr("Recv xml : %s\n", (const char*)pMSG->data);
	TiXmlDocument doc;
	doc.Parse((const char*)pMSG->data);
	
	TiXmlElement* pRootElem = doc.RootElement();
	if(!pRootElem)
	{
		OutPutStr("Parse packet failed : %s\n", (const char*)pMSG->data);
		OutDebugLine();
		return ;
	}

	std::string strTagName = CXmlCom::GetTagName(pRootElem);
	if("Response" != strTagName)
	{
		OutPutStr("pRootElem name is : %s\n", strTagName.c_str());
		OutDebugLine();
		return ;
	}

	int code = CXmlCom::GetAttribtueInt(pRootElem, "code");
	std::string strErr = CXmlCom::GetAttribtue(pRootElem, "errmsg");
	if(0 ==  code)
	{
		OutPutStr("code = %d, errmsg = %s\n", code, strErr.c_str());
		OutDebugLine();
		return ;
	}

	TiXmlElement* pNoResultSQLElem = CXmlCom::GetFirstChildElement(pRootElem, "NoResultSQL");
	TiXmlElement* pSQLElem = CXmlCom::GetFirstChildElement(pNoResultSQLElem, "SQL");

	std::string SQL;
	while(pSQLElem)
	{
		SQL = CXmlCom::GetText(pSQLElem);
		if(-1 == m_dbHandle->Exectue(SQL.c_str()))
		{
			OutPutStr("SQL = %s\n errmsg = %s\n", SQL.c_str(), m_dbHandle->GetError().c_str());
			OutDebugLine();
		}
		pSQLElem = pSQLElem->NextSiblingElement();
	}

	isDownload = 1;
}

//注册动态库
RegPlugin(TSMDbMngeCli, TSM_PROID, TSM_DBMID);
//注册对应的处理类
AddPluginUnitClass(TSMDbMngeCli, CDBMangerClient, 0);