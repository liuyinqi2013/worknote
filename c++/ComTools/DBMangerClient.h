#ifndef _H_H_DBMANGERCLIENT
#define _H_H_DBMANGERCLIENT

#include "Thread.h"
#include "PPlatformPlugin.h"
#include <queue>

class CMySQLite3;
class CXmlIniFile;


class CDBMangerClient : public CPPlatformPluginUnit, public CommonThread
{
public:
	CDBMangerClient();
	virtual ~CDBMangerClient();

	virtual int Init(int CorS);
    virtual int UnInit();
    virtual int DoOnce();
    virtual int SetMessage(struct struI_CommonMSG* p);

protected:
	virtual BOOL Work(void);

private:
	void ClearQueue();
	void SendReq(int reqType, const vector<std::string>& vecTables);
	void Send2Server(std::string& strXml);
	void HandleResponse(struI_CommonMSG* pMSG);
private:
	CMySQLite3*  m_dbHandle;
	CXmlIniFile* m_IniHandle;
	CommonMutex* m_MutexLock;
	std::queue<struI_CommonMSG*> m_MsgQueue;

	std::string strHostId;
	std::string strHostIp;

	int isDownload;
	unsigned long  reqTime;
	std::vector<string> vecDownloadTabs;
	std::vector<string> vecUploadTabs;

	int iNetType;
	
};

#endif 