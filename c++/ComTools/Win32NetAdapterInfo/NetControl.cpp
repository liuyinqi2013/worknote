#include <map>

#ifdef WIN32
#include <WinSock2.h>
#include <IPHlpApi.h>
#include <Windows.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define popen _popen
#define pclose _pclose
#define QueryCPUCmd "wmic cpu list full"
#define QueryOSCmd  "systeminfo"
#define QueryNetInfo "ipconfig /all"
#define LINENUM  3
#else
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "LinuxBase.h"

#define QueryCPUCmd "dmidecode -t 4"
#define QueryOSCmd  "systeminfo"
#define QueryNetInfo "ifconfig"
#define LINENUM  4
#endif

#include "NetStdAfx.h"
#include "NetAdapter.h"
#include "NetControl.h"



class CInitSock
{
public:
	CInitSock()
	{
#ifdef WIN32
		WORD wVersionRequested;
		WSADATA wsaData;
		wVersionRequested = MAKEWORD(2, 2);
		WSAStartup(wVersionRequested, &wsaData);
#endif
	}
	~CInitSock()
	{
	}
};


CInitSock InitSock;

int CNetControl::Init()
{
	if(!GetNetConfig())
	{
		PRINTDEBUG;
		return -1;
	}
	//SetNetConnStatus(true);
	//StartFireWall();
	//DeleteFireWallRule();
	//AddFireWallRule();
	m_bInit = true;
	return 0;
}

int CNetControl::UnInit()
{
	if(m_bInit)
	{
		if(!IsConnectNet())
		{
			ConnectNet();
		}
		//m_VecNetConfInfo.clear();
		//DeleteFireWallRule();
		m_bInit = false;
	}
	return 0;
}

void CNetControl::DoCheck()
{
	if(-1 == m_MonTimes || (time(NULL) - m_StartTime) < m_MonTimes * 60)
	{
		if(IsConnectNet())
		{
			DisconnectNet();
		}
	}
	else
	{
		if(!IsConnectNet())
		{
			ConnectNet();
		}
	}

}

bool CNetControl::DisconnectNet()
{
	CancelGateWay();
	//SetNetConnStatus(false);
	//EnableFireWallRules(true);
	m_bConn = false;
	return true;
}

bool CNetControl::ConnectNet()
{
	RestoreNetConfig();
	//SetNetConnStatus(true);
	//EnableFireWallRules(false);
	m_bConn = true;
	return true;
}

bool CNetControl::IsConnectNet()
{
	return m_bConn;
}

bool CNetControl::GetNetConfig()
{
#ifdef WIN32
	ULONG size(0);
	PIP_ADAPTER_INFO pinfo(NULL);
	int ret = GetAdaptersInfo(pinfo, &size);
	pinfo = (PIP_ADAPTER_INFO)malloc(size);
	ret = GetAdaptersInfo(pinfo, &size);
	while(pinfo)
	{
		m_NetConfInfo.strIpaddr = pinfo->IpAddressList.IpAddress.String;
		m_NetConfInfo.strMask = pinfo->IpAddressList.IpMask.String;
		m_NetConfInfo.strGateWayIp = pinfo->GatewayList.IpAddress.String;
		m_NetConfInfo.strDnsIp = pinfo->PrimaryWinsServer.IpAddress.String;
		m_NetConfInfo.strDes  = pinfo->Description;
		m_NetConfInfo.strAdapterName = pinfo->AdapterName;
		m_NetConfInfo.iIndex = pinfo->Index;
		if(pinfo->DhcpEnabled)
		{
			PRINTDEBUG;
			m_NetConfInfo.bDhcp = true;
		}
		else
		{
			PRINTDEBUG;
			m_NetConfInfo.bDhcp = false;
		}
		m_VecNetConfInfo.push_back(m_NetConfInfo);
		pinfo = pinfo->Next;
	}
	GetConnctionName();
#endif
	return true;
}


bool CNetControl::CancelGateWay()
{
	for(int i = 0; i < m_VecNetConfInfo.size(); ++i)
	{
		if("" == m_VecNetConfInfo[i].strConnName)
		{
			continue;
		}
		char strCmd[128] = {0};
		sprintf(strCmd, "netsh interface ip set address name=%d source=static addr=%s mask=%s ", m_VecNetConfInfo[i].iIndex, m_VecNetConfInfo[i].strIpaddr.c_str(),  m_VecNetConfInfo[i].strMask.c_str());
		printf("Exctue %s\n", strCmd);
		if(-1 == system(strCmd))
		{
			PRINTDEBUG;
			return false;
		}
		//CNetWorkAdapter NetAdapter(m_VecNetConfInfo[i].strAdapterName, m_VecNetConfInfo[i].strDes);
		//if(!NetAdapter.Open())
		//{
		//	continue;
		//}
		//if(m_VecNetConfInfo[i].bDhcp)
		//{
		//	NetAdapter.SetDhcp(false);
		//	NetAdapter.SetIpAddr(m_VecNetConfInfo[i].strIpaddr);
		//	NetAdapter.SetGateWay("");
		//	NetAdapter.SetDnsAddr(m_VecNetConfInfo[i].strDnsIp);
		//	NetAdapter.SetSubMask(m_VecNetConfInfo[i].strMask);
		//	NetAdapter.Flush();
		//}
		//else
		//{
		//	NetAdapter.SetGateWay("");
		//	NetAdapter.Flush();
		//}
		
	}
	return true;
}

bool CNetControl::RestoreNetConfig()
{
	for(int i = 0; i < m_VecNetConfInfo.size(); ++i)
	{
		if("" == m_VecNetConfInfo[i].strConnName)
		{
			continue;
		}
		char strCmd[128] = {0};
		char strCmd1[128] = {0};
		if(m_VecNetConfInfo[i].bDhcp)
		{
			sprintf(strCmd, "netsh interface ip set address name=%d source=dhcp", m_VecNetConfInfo[i].iIndex);
			sprintf(strCmd1, "netsh interface ip set dns name=%d source=dhcp", m_VecNetConfInfo[i].iIndex);
		}
		else
		{
			if(m_VecNetConfInfo[i].strGateWayIp == "" || m_VecNetConfInfo[i].strGateWayIp == "0.0.0.0"){
				sprintf(strCmd, "netsh interface ip set address name=%d source=static addr=%s mask=%s",m_VecNetConfInfo[i].iIndex, m_VecNetConfInfo[i].strIpaddr.c_str(), m_VecNetConfInfo[i].strMask.c_str());
			}
			else{
				sprintf(strCmd, "netsh interface ip set address name=%d source=static addr=%s mask=%s gateway=%s",m_VecNetConfInfo[i].iIndex, m_VecNetConfInfo[i].strIpaddr.c_str(), m_VecNetConfInfo[i].strMask.c_str(), m_VecNetConfInfo[i].strGateWayIp.c_str());
			}
			if(m_VecNetConfInfo[i].strDnsIp != "" && m_VecNetConfInfo[i].strDnsIp != "0.0.0.0") {
				sprintf(strCmd1, "netsh interface ip set dns name=%d source=static addr=%s register=primary", m_VecNetConfInfo[i].iIndex, m_VecNetConfInfo[i].strDnsIp.c_str());
			}
			else {
				//sprintf(strCmd1, "netsh interface ip set dns name=\"%s\" source=static addr=%s register=primary", m_VecNetConfInfo[i].strConnName.c_str(), m_VecNetConfInfo[i].strDnsIp.c_str());
				sprintf(strCmd1, "netsh interface ip set dns name=%d source=dhcp", m_VecNetConfInfo[i].iIndex);
			}
		}
		printf("Exctue %s\n", strCmd);
		if(-1 == system(strCmd))
		{
			PRINTDEBUG;
			return false;
		}
		printf("Exctue %s\n", strCmd1);
		if(-1 == system(strCmd1))
		{
			PRINTDEBUG;
			return false;
		}

		//CNetWorkAdapter NetAdapter(m_VecNetConfInfo[i].strAdapterName, m_VecNetConfInfo[i].strDes);
		//if(!NetAdapter.Open())
		//{
		//	continue;
		//}
		//if(m_VecNetConfInfo[i].bDhcp)
		//{
		//	NetAdapter.SetDhcp(true);
		//}
		//else
		//{
		//	NetAdapter.SetGateWay(m_VecNetConfInfo[i].strGateWayIp);
		//}
		//NetAdapter.Flush();
	}
	return true;
}


bool CNetControl::SetNetConnStatus(bool bEnabled)
{
	int pos(0);
	string ipTmp;
	string ip1;
	string ip2;
	string ipMin("255.255.255.255");
	string ipMax("0.0.0.0");

	map<unsigned long, string> ipMaps;
	
	ipMaps.insert(make_pair(htonl(inet_addr(ipMax.c_str())), ipMax));
	ipMaps.insert(make_pair(htonl(inet_addr(ipMin.c_str())), ipMin));
	for(int i = 0; i < m_VecNetConfInfo.size(); ++i)
	{
		pos = m_VecNetConfInfo[i].strIpaddr.rfind(".");
		if(pos == string::npos)
		{
			continue;
		}
		ipTmp = m_VecNetConfInfo[i].strIpaddr.substr(0, pos + 1);
		ip1 = ipTmp + string("0");
		ip2 = ipTmp + string("255");
		ipMaps.insert(make_pair(htonl(inet_addr(ip1.c_str())), ip1));
		ipMaps.insert(make_pair(htonl(inet_addr(ip2.c_str())), ip2));
		//if(htonl(inet_addr(ip1.c_str())) < htonl(inet_addr(ipMin.c_str())) 
		//	&& inet_addr(ip1.c_str()) > 0)
		//{
		//	ipMin = ip1;
		//}
		//if(htonl(inet_addr(ipMax.c_str())) < htonl(inet_addr(ip2.c_str())))
		//{
		//	ipMax = ip2;
		//}
	}
	map<unsigned long, string>::iterator it = ipMaps.begin();
	map<unsigned long, string>::iterator it1 = ipMaps.begin();
	int Index = 1;
	while(it != ipMaps.end() && ++it1 != ipMaps.end())
	{
		char strCmd[128] = {0};
		if(bEnabled)
		{
			sprintf(strCmd, "netsh advfirewall firewall add rule name=\"NetBlock%d\" dir=out remoteip", NetCtlName, 1, it->second.c_str(), it1->second.c_str(), Index);
			//sprintf(strCmd, "%s %d %s %s", NetCtlName, 1, ipMin.c_str(), ipMax.c_str());
		}
		else
		{
			sprintf(strCmd, "%s %d %s %s NetBlock%d", NetCtlName, 0, it->second.c_str(), it1->second.c_str(), Index);
			//sprintf(strCmd, "%s %d %s %s", NetCtlName, 0, ipMin.c_str(), ipMax.c_str());
		}

		printf("Exctue %s\n", strCmd);
		if(-1 == system(strCmd))
		{
			PRINTDEBUG;
			//return false;
		}
		++it,++it;
		it1 = it;
		++Index;
	}

	return true;
}

bool CNetControl::GetConnctionName()
{
	FILE* fp = popen(QueryNetInfo, "r");
	if(NULL == fp)
	{
		PRINTDEBUG;
		return false;
	}

#ifdef WIN32
	char buf[256] = {0};
	char buf1[256] = {0};
	char buf2[256] = {0};
	int i;
	while(fgets(buf, sizeof(buf), fp))
	{
		if(strlen(buf) < 10) continue;
		for(i = 0; i < m_VecNetConfInfo.size(); ++i)
		{
			if(strstr(buf, m_VecNetConfInfo[i].strDes.c_str()))
			{
				char *pHead = strchr(buf2, ' ');
				if(NULL == pHead) continue;
				char *pEnd = strchr(pHead++, ':');
				if(NULL == pEnd) continue;
				*pEnd = '\0';
				m_VecNetConfInfo[i].strConnName = pHead;
			}
		}
		memcpy(buf2, buf1, sizeof(buf2));
		memcpy(buf1, buf, sizeof(buf1));
		memset(buf, 0, sizeof(buf));
	}
#else
#endif
	pclose(fp);
	return true;
}


bool CNetControl::StartFireWall()
{
	printf("Exctue %s\n", AUTO_FIREWALL);
	if(-1 == system(AUTO_FIREWALL))
	{
		PRINTDEBUG;
		return false;
	}
	printf("Exctue %s\n", START_FIREWALL);
	if(-1 == system(START_FIREWALL))
	{
		PRINTDEBUG;
		return false;
	}
	printf("Exctue %s\n", START_FIREWALL_MONITOR);
	if(-1 == system(START_FIREWALL_MONITOR))
	{
		PRINTDEBUG;
		return false;
	}

	return true;
}

bool CNetControl::CloseFireWall()
{
	printf("Exctue %s\n", CLOSE_FIREWALL);
	if(-1 == system(CLOSE_FIREWALL))
	{
		PRINTDEBUG;
		return false;
	}

	return true;
}


bool CNetControl::AddFireWallRule()
{
	int pos(0);
	string ipTmp;
	string ip1;
	string ip2;
	string ipMin("255.255.255.255");
	string ipMax("0.0.0.0");
	map<unsigned long, string> ipMaps;
	ipMaps.insert(make_pair(htonl(inet_addr(ipMax.c_str())), ipMax));
	ipMaps.insert(make_pair(htonl(inet_addr(ipMin.c_str())), ipMin));
	for(int i = 0; i < m_VecNetConfInfo.size(); ++i)
	{
		pos = m_VecNetConfInfo[i].strIpaddr.rfind(".");
		if(pos == string::npos)
		{
			continue;
		}
		ipTmp = m_VecNetConfInfo[i].strIpaddr.substr(0, pos + 1);
		ip1 = ipTmp + string("0");
		ip2 = ipTmp + string("255");
		ipMaps.insert(make_pair(htonl(inet_addr(ip1.c_str())), ip1));
		ipMaps.insert(make_pair(htonl(inet_addr(ip2.c_str())), ip2));
	}

	map<unsigned long, string>::iterator it = ipMaps.begin();
	map<unsigned long, string>::iterator it1 = ipMaps.begin();
	int Index = 1;
	char strCmd[128] = {0};
	while(it != ipMaps.end() && ++it1 != ipMaps.end())
	{
		sprintf(strCmd, "netsh advfirewall firewall add rule name=\"%s\" dir=out enable=no remoteip=%s-%s action=block", m_strNetRule.c_str(), it->second.c_str(), it1->second.c_str());
		printf("Exctue %s\n", strCmd);
		if(-1 == system(strCmd))
		{
			PRINTDEBUG;
		}
		++it,++it;
		it1 = it;
		memset(strCmd, 0, sizeof(strCmd));
	}
	return true;
}


bool CNetControl::DeleteFireWallRule()
{
	char strCmd[128] = {0};
	sprintf(strCmd, "netsh advfirewall firewall delete rule name=\"%s\"", m_strNetRule.c_str());
	printf("Exctue %s\n", strCmd);
	if(-1 == system(strCmd))
	{
		PRINTDEBUG;
		return false;
	}
	return true;
}

bool CNetControl::EnableFireWallRules(bool bEnabled)
{
	string strEnable;
	if(bEnabled)
	{
		strEnable = "yes";
	}
	else
	{
		strEnable = "no";
	}
	
	char strCmd[128] = {0};
	sprintf(strCmd, "netsh advfirewall firewall set rule name=\"%s\" new enable=%s", m_strNetRule.c_str(), strEnable.c_str());
	printf("Exctue %s\n", strCmd);
	if(-1 == system(strCmd))
	{
		PRINTDEBUG;
		return false;
	}
	return true;
}