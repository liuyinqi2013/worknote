#include "NetAdapter.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib, "setupapi.lib")

typedef BOOL (WINAPI *DHCPNOTIFYCONFIGCHANGE)(LPWSTR, LPWSTR, BOOL, DWORD, DWORD, DWORD, int);

bool NotifyIPChange(const char *adapterName, char *ipAddr,  char *ipNetMask)
{ 
	bool bRes = false;
	WCHAR wcAdapterName[256] = {0};
	MultiByteToWideChar(CP_ACP, 0, adapterName, -1, wcAdapterName, 256);
	
	HINSTANCE hDhcpDll = LoadLibrary("dhcpcsvc");
	if( NULL == hDhcpDll)
	{
			printf("call LoadLibrary failed\n");
			FreeLibrary(hDhcpDll);
			return bRes;
	}

	DHCPNOTIFYCONFIGCHANGE pFunc = (DHCPNOTIFYCONFIGCHANGE)GetProcAddress(hDhcpDll, "DhcpNotifyConfigChange");
	if(NULL == pFunc)
	{
			printf("call GetProcAddress failed\n");
			FreeLibrary(hDhcpDll);
			return bRes;
	}

	int ret = pFunc(NULL, wcAdapterName, TRUE, 0, inet_addr("172.16.0.74"), inet_addr("255.255.255.0"), 1);
	if( ret != ERROR_SUCCESS)
	{
		printf("call DHCPNOTIFYCONFIGCHANGE failed error : %d\n", ret);
		FreeLibrary(hDhcpDll);
		return false;
	}
	FreeLibrary(hDhcpDll);
	return true;

}

bool CNetWorkAdapter::Open()
{
	if(!m_bOpen)
	{
		string strPathName = string("SYSTEM\\ControlSet001\\Services\\Tcpip\\Parameters\\Interfaces\\") +m_strAdapterName;
		if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, strPathName.c_str(), 0, KEY_WRITE, &m_RegKey))
		{
			return false;
		}

		m_DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		bool bfind(false);
		for(int i = 0; SetupDiEnumDeviceInfo(m_hDevinfo, i, &m_DevInfoData); i++)
		{
			DWORD data;
			LPTSTR p;
			char buffer[512] = {0};
			DWORD buffSize = 512;
			if(SetupDiGetDeviceRegistryProperty(m_hDevinfo, &m_DevInfoData, SPDRP_DEVICEDESC, &data, (PBYTE)buffer, buffSize, &buffSize))
			{
				printf("len = %d, %s\n", buffSize, buffer);
				if(strcmp(m_strAdapterDes.c_str(), buffer) == 0)
				{
					bfind = true;
					break;
				}
			}

		}

		if(!bfind)
		{
			return false;
		}
	}

	return (m_bOpen = true);
}

bool CNetWorkAdapter::SetIpAddr(const string& ipAddr)
{
	char buf[64] = {0};
	strcpy(buf, ipAddr.c_str());
	int len = ipAddr.size() + 2; //注意 ： REG_MULTI_SZ 类型后面要多加一个0x00
	if(ERROR_SUCCESS != RegSetValueEx(m_RegKey, "IPAddress", 0, REG_MULTI_SZ, (const BYTE*)buf, len))
	{
		return false;
	}
	return true;
}

bool CNetWorkAdapter::SetGateWay(const string& gateWayAddr)
{
	char buf[64] = {0};
	strcpy(buf, gateWayAddr.c_str());
	int len = gateWayAddr.size() + 2; //注意 ： REG_MULTI_SZ 类型后面要多加一个0x00
	if(ERROR_SUCCESS != RegSetValueEx(m_RegKey, "DefaultGateway", 0, REG_MULTI_SZ, (const BYTE*)buf, len))
	{
		return false;
	}
	return true;
}


bool CNetWorkAdapter::SetDhcp(bool enable)
{
	DWORD value = enable ? 1 : 0;
	if(ERROR_SUCCESS != RegSetValueEx(m_RegKey, "EnableDHCP", 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD)))
	{
		return false;
	}
	return true;
}

bool CNetWorkAdapter::SetDnsAddr(const string& dnsAddr)
{
	if(ERROR_SUCCESS != RegSetValueEx(m_RegKey, "NameServer", 0, REG_SZ, (const BYTE*)dnsAddr.c_str(), dnsAddr.size()))
	{
		return false;
	}
	return true;
}

bool CNetWorkAdapter::SetSubMask(const string& subMask)
{
	char buf[64] = {0};
	strcpy(buf, subMask.c_str());
	int len = subMask.size() + 2; //注意 ： REG_MULTI_SZ 类型后面要多加一个0x00
	if(ERROR_SUCCESS != RegSetValueEx(m_RegKey, "SubnetMask", 0, REG_MULTI_SZ, (const BYTE*)buf, len))
	{
		return false;
	}
	return true;
}

bool CNetWorkAdapter::Flush()
{
	//return NotifyIPChange(m_strAdapterName.c_str(), NULL, NULL);
	return  ChangNetAdapterStatus(false) ? ChangNetAdapterStatus(true) : false;
}

bool CNetWorkAdapter::ChangNetAdapterStatus(bool bEnable)
{
#ifdef WIN32
	DWORD NewStatus;
	if(bEnable)
	{
		NewStatus = DICS_ENABLE;
	}
	else
	{
		NewStatus = DICS_DISABLE;
	}

	SP_PROPCHANGE_PARAMS spPRAMS;
	spPRAMS.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	spPRAMS.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	spPRAMS.Scope = DICS_FLAG_GLOBAL;
	spPRAMS.StateChange = NewStatus;

	int ret = SetupDiSetClassInstallParams(m_hDevinfo, &m_DevInfoData, (SP_CLASSINSTALL_HEADER *)&spPRAMS, sizeof(SP_PROPCHANGE_PARAMS));
	printf("return code = %d\n", ret);
	if(!ret)
	{
		printf("SetupDiSetClassInstallParams func failed\n");
		return false;
	}

	ret = SetupDiChangeState(m_hDevinfo, &m_DevInfoData);
	printf("return code = %d\n", ret);
#endif
	return true;
}