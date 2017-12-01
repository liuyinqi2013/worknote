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
	static vector<CWirelessNetWorkCard*>* GetWirelessNetWorkCardInstances(); //��ȡ��������ʵ���б�
	static void Destroy(); //�ͷ���Դ
public:
	bool  Open();          //������������
	bool  Close();		   //�ر���������
	bool  GetStatus();     //��ȡ����������ǰ״̬
	
private:
	CWirelessNetWorkCard(const GUID&,
		const string&,
		const SP_DEVINFO_DATA&);
	~CWirelessNetWorkCard();
	GUID  m_gUid;         //��������ClassGUID;
	string m_strDes;      //������������
	SP_DEVINFO_DATA m_DevInfoData;
	bool ChangWirelessNetWorkCardStatus(bool bEnable); //�ı���������״̬
private:
	static vector<CWirelessNetWorkCard*>* pVecWirelessNetWorkCard;
	static HDEVINFO hDevInfo;
	static bool     bLoad;
	static HANDLE   hClient;
};

#endif