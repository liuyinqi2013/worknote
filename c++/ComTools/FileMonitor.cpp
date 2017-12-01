#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>



#include "FileMonitor.h"


CFileMonitor* CFileMonitor::pFileMonitor = NULL;


CFileMonitor* CFileMonitor::GetInstance()
{
	if(NULL == pFileMonitor)
	{
		pFileMonitor = new CFileMonitor;
	}

	return pFileMonitor;
}

void CFileMonitor::DestoryInstance()
{
	if(pFileMonitor)
	{
		delete pFileMonitor;
		pFileMonitor = NULL;
	}
}

CFileMonitor::CFileMonitor() : m_bInit(false),
	m_iNotifyFd(0),
	m_iEpollFd(0)
{

}

CFileMonitor::~CFileMonitor()
{
	Uninit();
}


int CFileMonitor::Init()
{

	if(m_bInit)
	{
		return 0;	
	}

	m_mapMonitorEvent.clear();
	m_vecPrivRes.clear();


	if (pthread_mutex_init(&m_mutexLock, NULL) == -1)
	{
		return -1;
	}
	
	if((m_iNotifyFd = inotify_init1(IN_NONBLOCK)) == -1)
	{
		return -1;
	}

	if((m_iEpollFd = epoll_create(10)) == -1)
	{
		close(m_iNotifyFd);
		return -1;
	}

	struct epoll_event eEvent;
	memset(&eEvent, 0, sizeof(epoll_event));
	eEvent.events = EPOLLIN;
	eEvent.data.fd = m_iNotifyFd;

	if(epoll_ctl(m_iEpollFd, EPOLL_CTL_ADD, m_iNotifyFd, &eEvent) == -1)	
	{
		close(m_iEpollFd);
		close(m_iNotifyFd);
		return -1;
	}

	m_bInit = true;
	
	return 0;

}


int CFileMonitor::Uninit()
{
	if(!m_bInit)
	{
		return 0;
	}

	m_mapMonitorEvent.clear();
	m_vecPrivRes.clear();
	close(m_iNotifyFd);
	close(m_iEpollFd);

	if(pthread_mutex_destroy(&m_mutexLock) == -1)
	{
		return -1;
	}
	
	m_bInit = false;
	
	return 0;	
}


int CFileMonitor::AddMonitorFilePath(string& filePath)
{

	if(filePath.size() < 1)
	{
		return -1;
	}

	if(!IsDir(filePath))
	{
		return -1;
	}

	if(filePath[filePath.size() - 1] !=  '/')
	{
		filePath += "/";
	}
	
	int ret = inotify_add_watch(m_iNotifyFd, filePath.c_str(),  IN_CREATE | IN_DELETE |  IN_MOVED_FROM | IN_MOVED_TO | IN_CLOSE_WRITE);
	if(-1 == ret)
	{
		return -1;
	}

	pthread_mutex_lock(&m_mutexLock);
	stuEventData eventData;
	eventData.ifd = ret;
	eventData.strFilePath = filePath;
	m_mapMonitorEvent[ret] = eventData;
	pthread_mutex_unlock(&m_mutexLock);
	
	DIR *dir = opendir(filePath.c_str());
	if(NULL != dir)
	{
		string fileName;
		struct dirent* pDirent = readdir(dir);
		while(pDirent)
		{
			if(pDirent->d_name[0] != '.')
			{
				fileName = filePath + pDirent->d_name;
				AddMonitorFilePath(fileName);
			}
			pDirent = readdir(dir);
		}

		closedir(dir);
	}
	
	return 0;
}


int CFileMonitor::DelMonitorWatchFilePath(string& filePath)
{
	if(!m_bInit)
	{
		return -1;
	}
	
	map<int, stuEventData>::iterator it = Find(filePath);
	
	pthread_mutex_lock(&m_mutexLock);
	
	if(it != m_mapMonitorEvent.end())
	{
		if(0 == inotify_rm_watch(m_iNotifyFd, it->first))
		{
			m_mapMonitorEvent.erase(it);
		}
	}
	
	pthread_mutex_unlock(&m_mutexLock);

	return 0;

}


int CFileMonitor::GetFileOperRes(vector<stuFileOperRes>& vecRes)
{
	struct epoll_event eEvents[2];
	memset(eEvents, 0, sizeof(epoll_event) * 2);
	int n = epoll_wait(m_iEpollFd, eEvents, 2, -1);
	if (-1 == n)
	{
		return -1;
	}

	int i = 0;
	for(;i < n; ++i)
	{
		if((eEvents[i].data.fd == m_iNotifyFd) && (eEvents[i].events & EPOLLIN))
		{
			break;
		}
	}
	
	if(i == n)
	{
		return -1;
	}
	
	char eventBuf[2048];
	memset(eventBuf, 0, sizeof(eventBuf));
	int len = read(m_iNotifyFd, eventBuf, sizeof(eventBuf));
	if(len == -1)
	{
		return -1;
	}
	
	int offost = 0;
	while((offost + sizeof(inotify_event)) < len)
	{
		inotify_event* event = (inotify_event*)((char *)eventBuf + offost);
		offost += sizeof(inotify_event) + event->len;
		if(event->len  == 0 || event->name[0] == '.')
		{
			continue;
		}
		stuFileOperRes operRes;
		
		if((event->mask & IN_MOVED_FROM) || (event->mask & IN_MOVED_TO))
		{
			int i;
			for(i = 0; i < vecRes.size(); ++i)
			{
				if(vecRes[i].cooks == event->cookie){
					ParseEvent(event, vecRes[i]);
					break;
				}
			}
			
			if(i < vecRes.size())
			{
				continue;
			}

			
			for(i = 0; i < m_vecPrivRes.size(); ++i)
			{
				if(m_vecPrivRes[i].cooks == event->cookie){
					ParseEvent(event, m_vecPrivRes[i]);
					vecRes.push_back(m_vecPrivRes[i]);
					break;
				}
			}

			if(i < m_vecPrivRes.size())
			{
				continue;
			}

		}
	
		operRes.cooks = event->cookie;
		if(0 == ParseEvent(event, operRes))
		{
			vecRes.push_back(operRes);
		}
	}

	m_vecPrivRes = vecRes;
	
	return 0;
}



int CFileMonitor::ParseEvent(void *event, stuFileOperRes& operRes)
{

	inotify_event* ievent = (inotify_event*)(event);
	
	if(NULL == ievent)
	{
		return -1;
	}

	map<int, stuEventData>::iterator it = m_mapMonitorEvent.find(ievent->wd);

	if(it == m_mapMonitorEvent.end())
	{
		return -1;
	}

	if(IsDir(it->second.strFilePath + ievent->name))
	{
		operRes.iFileType = FDIR;
	}

	map<int, stuEventData>::iterator it2 = Find(it->second.strFilePath + ievent->name + "/");
	if(it2 != m_mapMonitorEvent.end())
	{
		operRes.iFileType = FDIR;
	}

	if(ievent->mask & IN_ACCESS)
	{

	}
	
	if(ievent->mask & IN_MODIFY)
	{

	}

	if(ievent->mask & IN_ATTRIB)
	{

	}

	if(ievent->mask & IN_CLOSE_WRITE)
	{
		operRes.iOpType = FILE_SAVE;
		operRes.strSrcPathName = it->second.strFilePath;
		operRes.strSrcFileName = ievent->name;

	}

	if(ievent->mask & IN_CLOSE_NOWRITE)
	{

	}

	if(ievent->mask & IN_OPEN)
	{

	}

	if(ievent->mask & IN_MOVED_FROM)
	{
		operRes.iOpType = FILE_MOVE;
		operRes.strSrcPathName = it->second.strFilePath;
		operRes.strSrcFileName = ievent->name;

		if(operRes.strDestPathName == operRes.strSrcPathName)
		{
			operRes.iOpType = FILE_RENAME;
		}

	}

	if(ievent->mask & IN_MOVED_TO)
	{
		operRes.iOpType = FILE_MOVE;
		
		operRes.strDestPathName = it->second.strFilePath;
		operRes.strDestFileName = ievent->name;

		if(operRes.strDestPathName == operRes.strSrcPathName)
		{
			operRes.iOpType = FILE_RENAME;		
		}

		if(IsDir(operRes.strDestPathName + operRes.strDestFileName)) {
			string strFilePath = operRes.strSrcPathName + operRes.strSrcFileName + "/";
			DelMonitorWatchFilePath(strFilePath);
			string strNewFilePath = operRes.strDestPathName + operRes.strDestFileName + "/";
			AddMonitorFilePath(strNewFilePath);
		}
	}

	if(ievent->mask & IN_CREATE)
	{
		operRes.iOpType = FILE_CREATE;
		operRes.strSrcPathName = it->second.strFilePath;
		operRes.strSrcFileName = ievent->name;

		string strtmp = operRes.strSrcPathName + operRes.strSrcFileName;
		AddMonitorFilePath(strtmp);
	}

	if(ievent->mask & IN_DELETE)
	{
		operRes.iOpType = FILE_DELETE; 
		operRes.strSrcPathName = it->second.strFilePath;
		operRes.strSrcFileName = ievent->name;
		string strFilePath = operRes.strSrcPathName +  operRes.strSrcFileName + "/";
		DelMonitorWatchFilePath(strFilePath);
	}

	if(ievent->mask & IN_DELETE_SELF)
	{

	}

	if(ievent->mask & IN_MOVE_SELF)
	{
	}

	return 0;
}


bool CFileMonitor::IsDir(const string& fileName)
{
	struct stat statBuf = {0};
	if(0 != stat(fileName.c_str(), &statBuf))
	{
		return false;
	}
	
	if(S_ISDIR(statBuf.st_mode))
	{
		return true;
	}

	return false;
}


int CFileMonitor::TestPrint(const stuFileOperRes& fileOp)
{
	printf("---------------------------------------\n");

	switch(fileOp.iOpType)
	{
	case FILE_CREATE:
		printf("File OP : CREATE\n");
		break;
	case FILE_DELETE:
		printf("File OP : DELETE\n");
		break;
	case FILE_MOVE:
		printf("File OP : MOVE\n");
		break;
	case FILE_RENAME:
		printf("File OP : RENAME\n");
		break;
	case FILE_SAVE:
		printf("File OP : SAVE\n");
		break;
	}
	printf("fileType = %s\n", fileOp.iFileType == FDIR ? "DIR" : "FILE");
	printf("srcPath = %s\n", fileOp.strSrcPathName.c_str());
	printf("srcFileName = %s\n", fileOp.strSrcFileName.c_str());

	printf("destPath = %s\n", fileOp.strDestPathName.c_str());
	printf("destFileName = %s\n", fileOp.strDestFileName.c_str());

	printf("---------------------------------------\n");
	
}


map<int, stuEventData>::iterator CFileMonitor::Find(const string& fileName)
{
	pthread_mutex_lock(&m_mutexLock);

	map<int, stuEventData>::iterator it =  m_mapMonitorEvent.begin();
	for(; it != m_mapMonitorEvent.end(); it++)
	{
		if(it->second.strFilePath == fileName)
		{
			break;
		}
	}

	pthread_mutex_unlock(&m_mutexLock);

	return it;
}




