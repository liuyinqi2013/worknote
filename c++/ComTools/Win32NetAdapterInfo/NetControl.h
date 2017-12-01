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
	bool DisconnectNet();  //断开互联网
	bool ConnectNet();     //连接互联网
	bool IsConnectNet();   //检查是否连接互联网
	
	bool GetNetConfig();   //获取网络配置
	bool CancelGateWay();  //取消主机网关地址
	bool RestoreNetConfig();  //回复网络配置

	bool SetNetConnStatus(bool bEnabled); //改变网络与互联网连接状态
	bool StartFireWall(); //开启防火墙
	bool CloseFireWall(); //关闭防火墙
	bool AddFireWallRule();//设置防火墙规则
	bool DeleteFireWallRule(); //删除防火墙规则
	bool EnableFireWallRules(bool bEnabled); //启用或停止防火墙规则
	bool GetConnctionName(); //获取网络连接名称能
private:
	int    m_MonTimes;       //断网时长
	time_t m_StartTime;      //开始时间
	NetConfInfo m_NetConfInfo;
	vector<NetConfInfo> m_VecNetConfInfo; //网络配置列表
	string m_strNetRule;

	bool   m_bInit;  //是否初始化
	bool   m_bConn;  //是否连接
};

#endif