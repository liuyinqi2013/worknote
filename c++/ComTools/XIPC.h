#ifndef __XIPC__H__
#define __XIPC__H__

#ifdef UNICODE
#undef UNICODE
#endif

#include <string>
#include <string.h>
#ifdef WIN32
#include <Windows.h>
#define  ptp_t HANDLE
#define  INVALID_PTP INVALID_HANDLE_VALUE
#else
#define  ptp_t int
#define  INVALID_PTP -1
#endif

class CXipc
{
public:
	CXipc(); 
	CXipc(const char* name, int mode = 0);
	~CXipc();
	int Open();
	int Open(const char* name, int mode = 0);
	int Create();
	int Create(const char* name, int mode = 0);
	int Read(void* buf,  int bufSize, int type);
	int Write(void* buf, int bufSize, int type);
	int Close();
private:
	bool CheckConnect();
private:
	int   m_iMode;
	bool  m_bServer;
	ptp_t m_Tid;
	std::string m_Name;
	bool  m_bConnect;
	bool  m_bReconnected;
#ifdef WIN32
	OVERLAPPED m_Over;
#endif
};

#endif