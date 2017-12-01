#ifndef __H__HOST_INFO_H
#define __H__HOST_INFO_H

#include <string>
#include <vector>
using namespace std;


#ifndef WIN32
#define DLL_EXPORT 
#else
 #ifndef DLL_EXPORT
 #define DLL_EXPORT __declspec(dllexport )
 #endif
#endif

DLL_EXPORT class CCpuInfo
{
public:
	CCpuInfo()
	{
	}
	~CCpuInfo()
	{
	}

	int Load();

	string GetCPUID() const
	{
		return strCPUID;
	}

	string GetCPUName() const 
	{
		return strCPUName;
	}

	string GetCompanyName() const
	{
		return strCompanyName;
	}

	int GetMaxClockSpeed() const
	{
		return iMaxClockSpeed;
	}

	int GetCoreNum() const
	{
		return iCoreNum;
	}

private:
	static bool   bLoad;
	static string strCPUID;
	static string strCPUName;
	static string strCompanyName;
	static int    iMaxClockSpeed;
	static int    iCoreNum;
};

struct NetWorkAdapter
{
	int    Index;
	string strName;
	string strDescription;
	string strMACAddress;      // 7427EA148AFF
	string strMACAddressType1; // 74-27-EA-14-8A-FF
	string strMACAddressType2; // 74:27:EA:14:8A:FF
	string strIPAddress;	
};

DLL_EXPORT class CNetWorkAdaptesInfo
{
public:
	CNetWorkAdaptesInfo()
	{
	}
	
	~CNetWorkAdaptesInfo()
	{
	}

	int Load();

	int GetAdapterCount() const
	{
		return vecAdapters.size();
	}

	NetWorkAdapter GetNetWorkAdapter(int index = 0)
	{
		return vecAdapters[index];
	}
private:
	static bool   bLoad;
	static vector<NetWorkAdapter> vecAdapters;
};


struct OSInfo
{
	OSInfo& operator=(const OSInfo& rhs)
	{
		strOSUID = rhs.strOSUID;
		strOSName = rhs.strOSName;
		strOsVersion = rhs.strOsVersion;
		strOSInstallTime = rhs.strOSInstallTime;
		strOSRunTime = rhs.strOSRunTime;
		strOsType = rhs.strOsType;
		return *this;
	}
	string strOSUID;
	string strOSName;
	string strOsVersion;
	string strOSInstallTime;
	string strOSRunTime;
	string strOsType;
};

DLL_EXPORT int GetControlledHostID(string& strHostID);

DLL_EXPORT int GetHostIpAddr(string& strHostIp);

DLL_EXPORT int GetHostMACAddr(string& strMAC);

DLL_EXPORT int GetOSInfo(struct OSInfo& osInfo);

DLL_EXPORT int GetLoginName(string& strUser);

DLL_EXPORT int GetHostName(string& strHostName);

DLL_EXPORT int GetCurrSysTime(string& strTime);


#endif 