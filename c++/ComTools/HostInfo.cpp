#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
#include <WinSock2.h>
#include <IPHlpApi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define popen _popen
#define pclose _pclose
#define QueryCPUCmd "wmic cpu list full"
#define QueryOSCmd  "systeminfo"
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
#define LINENUM  4
#endif
#include "HostInfo.h"

#define IsSpace(x) (((x) == ' ')||((x) == '\t')||((x) == '\v')||((x) == '\n')||((x) == '\b'))

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

bool   CCpuInfo::bLoad(false);
string CCpuInfo::strCPUID;
string CCpuInfo::strCPUName;
string CCpuInfo::strCompanyName;
int    CCpuInfo::iMaxClockSpeed(0);
int    CCpuInfo::iCoreNum(0);

bool   CNetWorkAdaptesInfo::bLoad(false);
vector<NetWorkAdapter> CNetWorkAdaptesInfo::vecAdapters;

void ClearSpace(string& str);
void FormatTime(string& strTime); 

CInitSock initSock;

string GetCPUId()
{
	char buf[32] = {0};
#ifdef WIN32
	char CpuName[16] = {0};
	UINT CpuID1 = 0;
	UINT CpuID2 = 0;
	__asm
	{
		mov eax, 0
		cpuid
		mov dword ptr CpuName[0], ebx
		mov dword ptr CpuName[4], edx
		mov dword ptr CpuName[8], ecx
		mov eax, 1
		cpuid 
		mov CpuID1, edx
		mov CpuID2, eax
	}
	sprintf(buf, "%08X%08X", CpuID1, CpuID2);
#endif
	return string(buf);
}

int CCpuInfo::Load()
{
	if(bLoad)
	{
		return 0;
	}

#ifdef WIN32
	HKEY hKey;
	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey))
	{
		return -1;
	}

	char buf[128] = {0};
	DWORD len = 128;
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "ProcessorNameString", NULL, NULL, (LPBYTE)buf, &len))
	{
		RegCloseKey(hKey);
		return -1;
	}
	strCPUName = buf;
	if(strCPUName.find("Intel") != string::npos) 
	{
			strCompanyName = "Intel";
	} 
	else if(strCPUName.find("AMD") != string::npos)
	{
		strCompanyName = "AMD";
	} 
	else 
	{
		strCompanyName = "Other";
	}

	unsigned int maxHZ = 0;
	len = 4;
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "~MHz", NULL, NULL, (LPBYTE)&maxHZ, &len))
	{
		RegCloseKey(hKey);
		return -1;
	}
	char tmp[32] = {0};
	sprintf(tmp, "%.1f", (float)maxHZ / 1000);
	iMaxClockSpeed = atoi(tmp);
	strCPUID = GetCPUId();
	SYSTEM_INFO sysinfo;
	memset(&sysinfo, 0, sizeof(SYSTEM_INFO));
	GetSystemInfo(&sysinfo);
	iCoreNum = sysinfo.dwNumberOfProcessors;
	RegCloseKey(hKey);
#else
	FILE* fp = popen(QueryCPUCmd, "r");
	if(fp == NULL)
	{
		return -1;
	}
	int n(0); 
	char buf[256] = {0};
	while(fgets(buf, sizeof(buf), fp) && n < LINENUM)
	{
		char *p(NULL);
		char *pHead(NULL);
		if((p = strstr(buf, "ID:")) != NULL)
		{
			strCPUID.clear();
			pHead = strchr(buf, ':') + 1;
			*p = '\0';
			while(*pHead != '\0')
			{
				if(!isspace(*pHead)) strCPUID += *pHead;
				++pHead;
			}
			++n;
		}
		else if((p = strstr(buf, "Version:")) != NULL)
		{
			pHead = strchr(buf, ':') + 1;
			while(isspace(*pHead)) ++pHead;
			p = strchr(pHead, '\n');
			*p = '\0';

			strCPUName = pHead;
			if(strstr(pHead, "Intel")) {
				strCompanyName = "Intel";
			} else if(strstr(pHead, "AMD")) {
				strCompanyName = "AMD";
			} else {
				strCompanyName = "Other";
			}
			++n;
		}
		else if((p = strstr(buf, "Max Speed:")) != NULL)
		{
			pHead = strchr(buf, ':') + 1;
			while(isspace(*pHead)) ++pHead;
			p = strchr(pHead, ' ');
			*p = '\0';
			iMaxClockSpeed = atoi(pHead) / 10000;
			++n;
		}
		else if((p = strstr(buf, "Core Count:")) != NULL)
		{
			pHead = strchr(buf, ':') + 1;
			while(isspace(*pHead)) ++pHead;
			p = strchr(pHead, '\n');
			*p = '\0';
			iCoreNum = atoi(pHead);
			++n;
		}
		memset(buf, 0, sizeof(buf));
	}
	pclose(fp);
#endif
	bLoad = true;
	return 0;
}


int CNetWorkAdaptesInfo::Load()
{
	if(bLoad)
	{
		return 0;
	}
#ifdef WIN32
	ULONG size(0);
	PIP_ADAPTER_INFO pinfo(NULL);
	int ret = GetAdaptersInfo(pinfo, &size);
	if(0 == size)
	{
		return -1;
	}
	pinfo = (PIP_ADAPTER_INFO)malloc(size);
	ret = GetAdaptersInfo(pinfo, &size);
	NetWorkAdapter tmp;
	while(pinfo)
	{
		tmp.Index = pinfo->Index;
		tmp.strName = pinfo->AdapterName;
		tmp.strDescription = pinfo->Description;
		char buf[32] = {0};
		sprintf(buf, "%02X%02X%02X%02X%02X%02X",
			pinfo->Address[0],
			pinfo->Address[1],
			pinfo->Address[2],
			pinfo->Address[3],
			pinfo->Address[4],
			pinfo->Address[5]);
		tmp.strMACAddress = buf;

		sprintf(buf, "%02X-%02X-%02X-%02X-%02X-%02X",
			pinfo->Address[0],
			pinfo->Address[1],
			pinfo->Address[2],
			pinfo->Address[3],
			pinfo->Address[4],
			pinfo->Address[5]);
		tmp.strMACAddressType1 = buf;

		sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
			pinfo->Address[0],
			pinfo->Address[1],
			pinfo->Address[2],
			pinfo->Address[3],
			pinfo->Address[4],
			pinfo->Address[5]);
		tmp.strMACAddressType2 = buf;

		tmp.strIPAddress = pinfo->IpAddressList.IpAddress.String;
		vecAdapters.push_back(tmp);
		pinfo = pinfo->Next;
	}	
#else
	struct ifreq ifr;
	struct ifconf ifc;
	char buf[2048] = {0};
	int ret = 0;
	int sock =  socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if(sock == -1)
	{
		return -1;
	}

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if(ioctl(sock, SIOCGIFCONF, &ifc) == -1)
	{
		return -1;
	}

	char MAC[32] = {0};
	char ip[32] = {0};
	NetWorkAdapter tmp;
	struct ifreq* it = ifc.ifc_req;
	int count = ifc.ifc_len / sizeof(struct ifreq);
	for(int i = 0; i < count; ++i, ++it)
	{
		strcpy(ifr.ifr_name, it->ifr_name);
		tmp.Index = 0;
		tmp.strName=ifr.ifr_name;
		if(ioctl(sock, SIOCGIFFLAGS, &ifr) == 0)
		{
			if(ifr.ifr_flags & IFF_LOOPBACK)
				continue;
		}
		
		if(ioctl(sock, SIOCGIFHWADDR, &ifr) == 0)
		{
			sprintf(MAC,"%02X%02X%02X%02X%02X%02X",
				(unsigned char)ifr.ifr_hwaddr.sa_data[0],
				(unsigned char)ifr.ifr_hwaddr.sa_data[1],
				(unsigned char)ifr.ifr_hwaddr.sa_data[2],
				(unsigned char)ifr.ifr_hwaddr.sa_data[3],
				(unsigned char)ifr.ifr_hwaddr.sa_data[4],
				(unsigned char)ifr.ifr_hwaddr.sa_data[5]);
			tmp.strMACAddress = MAC;

			sprintf(MAC,"%02X-%02X-%02X-%02X-%02X-%02X",
				(unsigned char)ifr.ifr_hwaddr.sa_data[0],
				(unsigned char)ifr.ifr_hwaddr.sa_data[1],
				(unsigned char)ifr.ifr_hwaddr.sa_data[2],
				(unsigned char)ifr.ifr_hwaddr.sa_data[3],
				(unsigned char)ifr.ifr_hwaddr.sa_data[4],
				(unsigned char)ifr.ifr_hwaddr.sa_data[5]);
			tmp.strMACAddressType1 = MAC;

			sprintf(MAC,"%02X:%02X:%02X:%02X:%02X:%02X",
				(unsigned char)ifr.ifr_hwaddr.sa_data[0],
				(unsigned char)ifr.ifr_hwaddr.sa_data[1],
				(unsigned char)ifr.ifr_hwaddr.sa_data[2],
				(unsigned char)ifr.ifr_hwaddr.sa_data[3],
				(unsigned char)ifr.ifr_hwaddr.sa_data[4],
				(unsigned char)ifr.ifr_hwaddr.sa_data[5]);
			tmp.strMACAddressType2 = MAC;
		}

		if(ioctl(sock, SIOCGIFADDR, &ifr) == 0)
		{
			struct sockaddr_in sin;
			memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
			tmp.strIPAddress = inet_ntoa(sin.sin_addr);
		}

		vecAdapters.push_back(tmp);
	}
#endif
	bLoad = true;
	return 0;
}


int GetControlledHostID(string& strHostID)
{
	//CCpuInfo CPU;
	/*if(CPU.Load() != 0)
	{
		return -1;
	}*/
	CNetWorkAdaptesInfo NetCaredInfo;
	if(NetCaredInfo.Load() != 0)
	{
		return -1;
	}
	//strHostID = CPU.GetCPUID();
	if(NetCaredInfo.GetAdapterCount() < 1)
	{
		return -1;
	}
	strHostID = NetCaredInfo.GetNetWorkAdapter().strMACAddress ;
	return 0;
}


int GetHostIpAddr(string& strHostIp)
{
	CNetWorkAdaptesInfo NetCaredInfo;
	if(NetCaredInfo.Load() != 0)
	{
		return -1;
	}
	strHostIp = NetCaredInfo.GetNetWorkAdapter().strIPAddress;
	return 0;
}

int GetHostMACAddr(string& strMAC)
{	
	CNetWorkAdaptesInfo NetCaredInfo;
	if(NetCaredInfo.Load() != 0)
	{
		return -1;
	}
	strMAC = NetCaredInfo.GetNetWorkAdapter().strMACAddressType2;
	return 0;
}


int GetLoginName(string& strUser)
{
#ifdef WIN32
	char buf[256] = {0};
	int len(256);
	if(!GetUserName(buf, (LPDWORD)&len))
	{
		return -1;
	}
	strUser = buf;
#else
	char *p = getlogin();
	if(p == NULL)
	{
		return -1;
	}
	strUser = p;
#endif
	return 0;
}

int GetHostName(string& strHostName)
{
	char buf[256] = {0};
	if(gethostname(buf, sizeof(buf)) < 0)
	{
		return -1;
	}

	strHostName = buf;
	return 0;
}


int GetCurrSysTime(string& strTime)
{
	time_t T;
	time(&T);
	struct tm *tmp = localtime(&T);
	if(tmp == NULL)
	{
		return -1;
	}

	char buf[256] = {0};
	sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d", 
		tmp->tm_year+1900,
		tmp->tm_mon+1,
		tmp->tm_mday,
		tmp->tm_hour,
		tmp->tm_min,
		tmp->tm_sec);
	strTime = buf;
	return 0;
}

struct OSInfo os;
static bool   bOsGet(false);

int GetOSInfo(struct OSInfo& osInfo)
{
	if(bOsGet)
	{
		osInfo = os;
		return 0;
	}
#ifdef WIN32
	FILE* fp = popen(QueryOSCmd, "r");
	if(fp == NULL)
	{
		return -1;
	}

	int n(0);
	int line(0);
	char buf[256] = {0};
	char* pHead(NULL);
	char* pEnd(NULL);
	fgets(buf, sizeof(buf), fp);
	while(fgets(buf, sizeof(buf), fp) && n < 5)
	{	
		if(line != 1 && line != 2 && line != 8 && line != 9 && line != 10)
		{
			++line;
			continue;

		}
 		if((pHead = strchr(buf, ':')) != NULL)
		{
			++pHead;
			while(IsSpace(*pHead)) ++pHead;
			pEnd = strrchr(pHead, '\n');
			while(IsSpace(*pEnd) && IsSpace(*(pEnd - 1))) --pEnd;
			*pEnd = '\0';
		}
		if(line == 1)
		{
			os.strOSName = pHead;
			n++;
		}
		else if(line == 2)
		{
			os.strOsVersion = pHead;
			n++;
		}
		else if(line == 8)
		{

			os.strOSUID = pHead;
			n++;
		}
		else if(line == 9)
		{
			os.strOSInstallTime = pHead;
			string tmp;
			for(int i = 0; i < os.strOSInstallTime.size(); ++i) {
				if(os.strOSInstallTime[i] == '/')
				{
					tmp += '-';
				}
				else if(os.strOSInstallTime[i] == ',')
				{
					continue;
				}
				else 
				{
					tmp += os.strOSInstallTime[i];
				}
			}
			os.strOSInstallTime = tmp;
			FormatTime(os.strOSInstallTime);
			n++;
		}
		else if(line == 10)
		{
			os.strOSRunTime = pHead;
			string tmp;
			for(int i = 0; i < os.strOSRunTime.size(); ++i) {
				if(os.strOSRunTime[i] == '/')
				{
					tmp += '-';
				}
				else if(os.strOSRunTime[i] == ',')
				{
					continue;
				}
				else 
				{
					tmp += os.strOSRunTime[i];
				}
			}
			os.strOSRunTime = tmp;
			FormatTime(os.strOSRunTime);
			n++;
		}
		++line;
		memset(buf, 0, sizeof(buf));
	}
	pclose(fp);
	os.strOsType = "Microsoft Windows";
	osInfo = os;
	bOsGet = true;
#else
	GetOsName(os.strOSName);
	GetOSVersion(os.strOsVersion);
	GetOSUID(os.strOSUID);
	GetOSInstallTime(os.strOSInstallTime);
	GetPowerOnTime(os.strOSRunTime);
	os.strOsType = "GNU/Linux";
	osInfo = os;
	bOsGet = true;
#endif

	return 0;
}

void ClearSpace(string& str)
{
	string tmp;
	for(int i = 0; i < str.size(); ++i)
	{
		if(!isspace(str[i]))
		{
			tmp += str[i];
		}

	}
	str = tmp;
}

void FormatTime(string& strTime)
{
	char buf[128] = {0};
	strcpy(buf, strTime.c_str());
	int year(0), mon(0), day(0), hour(0), Min(0), sec(0);
	sscanf(buf, "%d-%d-%d %d:%d:%d", &year, &mon, &day, &hour, &Min, &sec);
	sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, Min, sec);
	strTime = buf;
}