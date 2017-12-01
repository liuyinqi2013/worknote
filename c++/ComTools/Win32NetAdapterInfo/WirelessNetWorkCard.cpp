#include "WirelessNetWorkCard.h"

vector<CWirelessNetWorkCard*>* CWirelessNetWorkCard::pVecWirelessNetWorkCard = NULL;
HDEVINFO CWirelessNetWorkCard::hDevInfo = NULL;
bool     CWirelessNetWorkCard::bLoad(false);
HANDLE   CWirelessNetWorkCard::hClient = NULL;


vector<CWirelessNetWorkCard*>* CWirelessNetWorkCard::GetWirelessNetWorkCardInstances()
{
	printf("GetWirelessNetWorkCardInstances Start\n");
	if(!bLoad)
	{
	   Destroy();
	   DWORD dwMaxClient = 2;
	   DWORD dwCurVersion = 0;
	   DWORD dwResult = 0;
	   DWORD dwRestVal = 0;

	   PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	   PWLAN_INTERFACE_INFO pInfo = NULL;

	   dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
	   if(ERROR_SUCCESS != dwResult)
	   {
		   switch(dwResult)
		   {
		   case ERROR_INVALID_PARAMETER:
			   printf("pdwNegotiatedVersion is NULL, phClientHandle is NULL, or pReserved is not NULL.\n");
			    break;
		   case ERROR_NOT_ENOUGH_MEMORY:
			   printf("Failed to allocate memory to create the client context.\n");
			    break;
		   case ERROR_REMOTE_SESSION_LIMIT_EXCEEDED:
			   printf("Too many handles have been issued by the server.\n");
			    break;
		   default:
			   printf("Other error\n");
			   break;
		   }

		   printf("call WlanOpenHandle func failed\n");
		   return NULL;
	   }

	   dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
	   if(ERROR_SUCCESS != dwResult)
	   {
		   WlanCloseHandle(hClient, NULL);
		   printf("call WlanEnumInterfaces func failed\n");
		   return NULL;
	   }

	   	hDevInfo = SetupDiGetClassDevs(NULL, "USB", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
		if(hDevInfo == INVALID_HANDLE_VALUE)
		{
			printf("SetupDiGetClassDevs func failed\n");
			return NULL;
		}

		DWORD data;
		char buffer[512] = {0};
		char buf[512] = {0};
		DWORD buffSize = 512;
		SP_DEVINFO_DATA DevInfoData;
		DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		pVecWirelessNetWorkCard = new vector<CWirelessNetWorkCard*>;
		for(int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DevInfoData); i++)
		{
			if(SetupDiGetDeviceRegistryProperty(hDevInfo, &DevInfoData, SPDRP_DEVICEDESC, &data, (PBYTE)buffer, buffSize, &buffSize))
			{
				//printf("buffer:%s\n", buffer);
				for(int k = 0; k <  pIfList->dwNumberOfItems; ++k)
				{
					size_t len = wcstombs(buf, pIfList->InterfaceInfo[k].strInterfaceDescription, 512);
					if(-1 == len) continue;
					buf[len] = '\0'; 
					//printf("buf:%s\n", buf);
					if(strcmp(buf, buffer) == 0)
					{
						printf("DES:%s\n", buf);
						CWirelessNetWorkCard*  pNetWorkCard = new CWirelessNetWorkCard(pIfList->InterfaceInfo[i].InterfaceGuid, buf, DevInfoData);	
						pVecWirelessNetWorkCard->push_back(pNetWorkCard);
					}
					memset(buf, 0, 512);
				}

			}
			memset(buffer, 0, 512);
			buffSize = 512;
		}

		bLoad = true;
	}
	printf("GetWirelessNetWorkCardInstances end\n");
	return pVecWirelessNetWorkCard;
}

void CWirelessNetWorkCard::Destroy()
{
	printf("Destroy start\n");
	if(bLoad && pVecWirelessNetWorkCard)
	{
		vector<CWirelessNetWorkCard*>::iterator it = pVecWirelessNetWorkCard->begin();
		for( ; it != pVecWirelessNetWorkCard->end(); it++)
		{
			if(*it)
			{
				delete *it;
				*it = NULL;
			}
		}
		WlanCloseHandle(hClient, NULL);
		pVecWirelessNetWorkCard->clear();
		delete pVecWirelessNetWorkCard;
		pVecWirelessNetWorkCard = NULL;
		bLoad = false;
	}
	printf("Destroy end\n");
}


CWirelessNetWorkCard::CWirelessNetWorkCard(const GUID& guid, const string& strDes, const SP_DEVINFO_DATA&  devInfo)
{
	memcpy(&(this->m_gUid), &(guid), sizeof(GUID));
	this->m_strDes = strDes;
	memcpy(&(this->m_DevInfoData), &(devInfo), sizeof(SP_DEVINFO_DATA));
}

CWirelessNetWorkCard::~CWirelessNetWorkCard()
{
}


bool  CWirelessNetWorkCard::Open()
{
	return ChangWirelessNetWorkCardStatus(true);
}

bool  CWirelessNetWorkCard::Close()
{
	return ChangWirelessNetWorkCardStatus(false);
}


bool CWirelessNetWorkCard::ChangWirelessNetWorkCardStatus(bool bEnable)
{
	printf("ChangWirelessNetWorkCardStatus start\n");
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

	int ret = SetupDiSetClassInstallParams(hDevInfo, &m_DevInfoData, (SP_CLASSINSTALL_HEADER *)&spPRAMS, sizeof(SP_PROPCHANGE_PARAMS));
	printf("return code = %d\n", ret);
	if(!ret)
	{
		printf("SetupDiSetClassInstallParams func failed\n");
		return false;
	}

	ret = SetupDiChangeState(hDevInfo, &m_DevInfoData);
	printf("return code = %d\n", ret);
	if(!ret)
	{
		printf("SetupDiSetClassInstallParams func failed\n");
		return false;
	}
	printf("ChangWirelessNetWorkCardStatus end\n");
	return true;
}

bool CWirelessNetWorkCard::GetStatus()
{
	return true;
}