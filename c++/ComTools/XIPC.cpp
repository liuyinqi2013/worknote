#include <time.h>
#include <stdlib.h>   
#include <stdio.h>

#ifndef WIN32
#include <unistd.h> 
#include <errno.h>   
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#endif

#include "XIPC.h"

struct xmsgbuf {
#ifndef WIN32
	long mtype;       /* message type, must be > 0 */
#endif
	int  len;
    char data[4096];    /* message data */
};


using namespace std;


CXipc::CXipc() : 
	m_iMode(0),
	m_bServer(false),
	m_Tid(INVALID_PTP),
	m_bReconnected(false)
{
}

CXipc::CXipc(const char* name, int mode) :
	m_iMode(mode),
	m_bServer(false),
	m_Tid(INVALID_PTP),
	m_bReconnected(false)
{
#ifdef WIN32
	m_Name = std::string("\\\\.\\pipe\\") + std::string(name);
#else
	m_Name = std::string("/tmp/") + std::string(name);
#endif
}

CXipc::~CXipc() 
{
}

int CXipc::Create(const char* name, int mode)
{
#ifdef WIN32
	m_Name = std::string("\\\\.\\pipe\\") + std::string(name);
#else
	m_Name = std::string("/tmp/") + std::string(name);
#endif
	m_iMode = mode;
	return Create();
}

int CXipc::Open(const char* name, int mode)
{
#ifdef WIN32
	m_Name = std::string("\\\\.\\pipe\\") + std::string(name);
#else
	m_Name = std::string("/tmp/") + std::string(name);
#endif
	m_iMode = mode;
	return Open();
}


#ifdef WIN32

int CXipc::Create()
{
    m_Tid = CreateNamedPipe(m_Name.c_str(), PIPE_ACCESS_DUPLEX  | FILE_FLAG_OVERLAPPED, m_iMode, 1, 4096, 4096, 0, NULL);
	if(INVALID_HANDLE_VALUE == m_Tid)
	{
		return -1;
	}

	memset(&m_Over, 0, sizeof(OVERLAPPED));
	HANDLE hevent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!hevent)
	{
		CloseHandle(m_Tid);
		return -1;
	}
	m_Over.hEvent = hevent;

	if(!ConnectNamedPipe(m_Tid, &m_Over))
	{
		if(ERROR_IO_PENDING != GetLastError() && m_iMode == 0)
		{
			return -1;
		}
	}

	WaitForSingleObject(m_Over.hEvent, 200);

	m_bConnect = true;
	m_bReconnected = false;
	m_bServer = true;
	return 0;
}

int CXipc::Open()
{
	if(!WaitNamedPipe(m_Name.c_str(), /*NMPWAIT_USE_DEFAULT_WAIT*/100))
	{
		return Create();
	}

	m_Tid = CreateFile(m_Name.c_str(), GENERIC_READ | GENERIC_WRITE , 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == m_Tid)
	{
		return -1;
	}

	m_bConnect = true;
	m_bReconnected = false;
	m_bServer = false;
	return 0;
}

int CXipc::Read(void* buf,  int bufSize, int type)
{
	if(buf == NULL || m_Tid == INVALID_PTP || !CheckConnect())
	{
		return -1;
	}

	struct xmsgbuf tmp;
	memset(&tmp, 0, sizeof(struct xmsgbuf));
	int len(0);
	if(!ReadFile(m_Tid, &tmp, sizeof(struct xmsgbuf), (LPDWORD)&len,  NULL))
	{
		if(m_bReconnected && !m_iMode)
			m_bConnect = false;
		return -1;
	}
	else
	{
		m_bReconnected = true;
	}

	int size = bufSize > tmp.len ? tmp.len : bufSize;
	memcpy(buf, tmp.data, size);

	return size;
}


int CXipc::Write(void* buf,  int bufSize, int type)
{
	if(buf == NULL || m_Tid == INVALID_PTP || !CheckConnect())
	{
		return -1;
	}
	struct xmsgbuf tmp;
	memset(&tmp, 0, sizeof(struct xmsgbuf));
	tmp.len = bufSize > sizeof(tmp.data) ? sizeof(tmp.data) : bufSize;
	memcpy(tmp.data, buf, tmp.len);
	int len(0);
	if(!WriteFile(m_Tid, &tmp, sizeof(struct xmsgbuf), (LPDWORD)&len, NULL))
	{
		if(m_bReconnected && !m_iMode)
			m_bConnect = false;
		return -1;
	}
	else
	{
		m_bReconnected = true;
	}
	return len;
}

int CXipc::Close()
{
	DisconnectNamedPipe(m_Tid);
	CloseHandle(m_Tid);
	CloseHandle(m_Over.hEvent);
	return 0;
}

bool CXipc::CheckConnect()
{
	if (m_bServer)
	{
		if(!m_bConnect) {
			Close();
			Create();
		}
	}
	else
	{
		if(!m_bConnect) {
			Open();
		}
	}
	return m_bConnect;
}

#else

int CXipc::Create()
{
	key_t key;
	key = ftok(m_Name.c_str(), 1);
	if(key == -1)
	{
		FILE* fp = fopen(m_Name.c_str(), "w");
		if(fp == NULL) return -1;
		fclose(fp);
		key = ftok(m_Name.c_str(), 1);
		if(key == -1) return -1;
	}

	m_Tid = msgget(key, IPC_CREAT|0666);
	if(m_Tid < 0)
	{
		return -1;
	}

	m_bConnect = true;
	m_bReconnected = false;
	m_bServer = true;
	return 0;
}

int CXipc::Open()
{
	key_t key;
	key = ftok(m_Name.c_str(), 1);
	if(key == -1)
	{
		FILE* fp = fopen(m_Name.c_str(), "w");
		if(fp == NULL) return -1;
		fclose(fp);
		key = ftok(m_Name.c_str(), 1);
		if(key == -1) return -1;
	}

	m_Tid = msgget(key, IPC_CREAT|0666);
	if(m_Tid < 0)
	{
		return -1;
	}

	m_bConnect = true;
	m_bReconnected = false;
	m_bServer = false;
	return 0;
}

int CXipc::Read(void* buf,  int bufSize, int type)
{
	if(buf == NULL || m_Tid == INVALID_PTP || !CheckConnect())
	{
		return -1;
	}

	struct xmsgbuf m;
	memset(&m, 0, sizeof(struct xmsgbuf));
	int len = 0;
	int flag = m_iMode == 0 ? 0 : IPC_NOWAIT;
	len =  msgrcv(m_Tid, (void*)&m, sizeof(xmsgbuf) - sizeof(long),  type , flag);
	if(len == -1)
	{
		return -1;
	}

	len = m.len > bufSize ? bufSize : m.len;
	memcpy(buf, m.data, len);
	return len;
}

int CXipc::Write(void* buf,  int bufSize, int type)
{
	if(buf == NULL || m_Tid == INVALID_PTP || !CheckConnect())
	{
		return -1;
	}
	struct xmsgbuf m;
	memset(&m, 0, sizeof(xmsgbuf));
	int len = 0;
	if (bufSize > sizeof(m.data))
	{
		len = sizeof(m.data);
	}
	else
	{
		len = bufSize;
	}

	m.mtype = type;
	m.len = len;
	memcpy(m.data, buf, len);
	int flag = m_iMode == 0 ? 0 : IPC_NOWAIT;
	return msgsnd(m_Tid, (void *)&m, sizeof(m) - sizeof(long), flag); 
}

int CXipc::Close()
{
	return msgctl(m_Tid, IPC_RMID, 0);
}

bool CXipc::CheckConnect()
{
	return true;
}

#endif
