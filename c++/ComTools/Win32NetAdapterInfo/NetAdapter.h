#ifndef __H__NETADAPTER_H
#define __H__NETADAPTER_H
#include <string>
#include <Windows.h>
#include <SetupAPI.h>
#include "NetStdAfx.h"


using namespace std;


class CNetWorkAdapter
{
public:
	CNetWorkAdapter() :
		m_strAdapterName(""),
		m_strAdapterDes(""),
		m_hDevinfo(INVALID_HANDLE_VALUE),
		m_bOpen(false)
	{
		m_hDevinfo = SetupDiGetClassDevs(NULL, "PCI", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	}

	CNetWorkAdapter(const string& adapterName, const string& adapterDes) :
		m_strAdapterName(adapterName),
		m_strAdapterDes(adapterDes),
		m_hDevinfo(INVALID_HANDLE_VALUE),
		m_bOpen(false)
	{
		m_hDevinfo = SetupDiGetClassDevs(NULL, "PCI", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	}

	~CNetWorkAdapter()
	{
		Close();
	}

	bool Open();

	void Close()
	{
		if(m_bOpen)
		{
			RegCloseKey(m_RegKey);
			m_bOpen = false;
		}
	}

	void SetNetAdapter(const string& adapterName, const string& adapterDes)
	{
		if(m_strAdapterName != adapterName)
		{
			m_strAdapterName = adapterName;
			m_strAdapterDes = adapterDes;
			Close();
		}
	}

	string GetNetAdapterName() const
	{
		return m_strAdapterName;
	}

	string GetNetAdapterDes() const
	{
		return m_strAdapterDes;
	}

	bool IsOpen() const
	{
		return  m_bOpen;
	}

	bool SetIpAddr(const string& ipAddr);
	bool SetGateWay(const string& gateWayAddr);
	bool SetDhcp(bool enable);
	bool SetDnsAddr(const string& dnsAddr);
	bool SetSubMask(const string& subMask);
	bool Flush();
private:
	bool ChangNetAdapterStatus(bool bEnable);
private:
	string m_strAdapterName;
	string m_strAdapterDes;
	HKEY   m_RegKey;
	HDEVINFO m_hDevinfo;
	SP_DEVINFO_DATA m_DevInfoData;
	bool   m_bOpen;
};

#endif