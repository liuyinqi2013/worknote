#ifndef _H_WIRELESSNETWORKCARD
#define _H_WIRELESSNETWORKCARD

#include <vector>

#ifdef WIN32
#include <Windows.h>
#include <SetupAPI.h>
#include <Wlanapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "Wlanapi.lib")
#else
#endif

using namespace std;

class CWirelessNetWorkCard
{
public:
	static vector<CWirelessNetWorkCard*>* GetWirelessNetWorkCardInstances(); //获取无线网卡实例列表
	static void Destroy(); //释放资源
public:
	bool  Open();          //开启无线网卡
	bool  Close();		   //关闭无线网卡
	bool  GetStatus();     //获取无线网卡当前状态
	
private:
	CWirelessNetWorkCard(const GUID&,
		const string&,
		const SP_DEVINFO_DATA&);
	~CWirelessNetWorkCard();
	GUID  m_gUid;         //无线网卡ClassGUID;
	string m_strDes;      //无线网卡描述
	SP_DEVINFO_DATA m_DevInfoData;
	bool ChangWirelessNetWorkCardStatus(bool bEnable); //改变无线网卡状态
private:
	static vector<CWirelessNetWorkCard*>* pVecWirelessNetWorkCard;
	static HDEVINFO hDevInfo;
	static bool     bLoad;
	static HANDLE   hClient;
};

#endif