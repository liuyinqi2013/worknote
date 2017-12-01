#ifndef _H_NETMANGER_H
#define _H_NETMANGER_H

#include <string>
#include <time.h>

#include "NetStdAfx.h"
class CNetControl
{
public:
	CNetControl() :
		m_MonTimes(0),
		m_StartTime(0),
		m_NetConfInfo(),
		m_strNetRule("NetBlock"),
		m_bInit(false),
		m_bConn(true)
	{
	}

	~CNetControl()
	{
		//SetNetConnStatus(true);
		UnInit();
	}

	int Init();
	int UnInit();

	void DoCheck();

	void SetMonitorTimes(int times)
	{
		m_MonTimes = times;
		m_StartTime = time(NULL);
	}


private:
	bool DisconnectNet();  //�Ͽ�������
	bool ConnectNet();     //���ӻ�����
	bool IsConnectNet();   //����Ƿ����ӻ�����
	
	bool GetNetConfig();   //��ȡ��������
	bool CancelGateWay();  //ȡ���������ص�ַ
	bool RestoreNetConfig();  //�ظ���������

	bool SetNetConnStatus(bool bEnabled); //�ı������뻥��������״̬
	bool StartFireWall(); //��������ǽ
	bool CloseFireWall(); //�رշ���ǽ
	bool AddFireWallRule();//���÷���ǽ����
	bool DeleteFireWallRule(); //ɾ������ǽ����
	bool EnableFireWallRules(bool bEnabled); //���û�ֹͣ����ǽ����
	bool GetConnctionName(); //��ȡ��������������
private:
	int    m_MonTimes;       //����ʱ��
	time_t m_StartTime;      //��ʼʱ��
	NetConfInfo m_NetConfInfo;
	vector<NetConfInfo> m_VecNetConfInfo; //���������б�
	string m_strNetRule;

	bool   m_bInit;  //�Ƿ��ʼ��
	bool   m_bConn;  //�Ƿ�����
};

#endif