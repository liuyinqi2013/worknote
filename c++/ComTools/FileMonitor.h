#ifndef _H_H_FILEMONITOR
#define _H_H_FILEMONITOR

#include <map>
#include <string>
#include <vector>

#include <pthread.h>

//文件操作类型
#define FILE_CREATE 1   //创建文件
#define FILE_DELETE 2   //删除文件
#define FILE_MOVE   3   //移到文件
#define FILE_RENAME 4   //重命名文件
#define FILE_SAVE   5   //保存文件

//文件名称

#define FNORMAL 1   //普通文件
#define FDIR    2   //文件夹

using namespace std;

struct stuFileOperRes
{
	stuFileOperRes() :
		iOpType(0),
		strSrcPathName(""),
		strDestPathName(""),
		strSrcFileName(""),
		strDestFileName(""),
		strDesc(""),
		cooks(0),
		iFileType(FNORMAL)
	{
	}
		
	int    iOpType;         //文件操作类型
	string strSrcPathName;  //源文件路径
	string strDestPathName; //目的文件路径
	string strSrcFileName;  //源文件名
	string strDestFileName; //目的文件名
	string strDesc;
	
	int cooks;
	int iFileType;          //文件类型
};

struct stuEventData
{
	int ifd;
	int iFileType;
	string strFilePath;
};

class CFileMonitor
{
public:
	static CFileMonitor* GetInstance();
	static void DestoryInstance();
	
public:
	CFileMonitor();
	~CFileMonitor();

	int Init();   //使用前调用初始化
	int Uninit(); //使用后调用反初始化 
	
	int AddMonitorFilePath(string& filePath);      //增加监控文件夹、必须是文件夹。
	int DelMonitorWatchFilePath(string& filePath); //删除监控的文件夹。

	int GetFileOperRes(vector<stuFileOperRes>& vecRes); //获取操作结果，没有结果会一直阻塞。

	int TestPrint(const stuFileOperRes& fileOp);

private:

	int ParseEvent(void *event, stuFileOperRes& operRes);

	bool IsDir(const string& fileName);

	map<int, stuEventData>::iterator Find(const string& fileName);
	
private:
	int m_bInit;
	int m_iNotifyFd;
	int m_iEpollFd;

	map<int, stuEventData> m_mapMonitorEvent;
	pthread_mutex_t m_mutexLock;

	vector<stuFileOperRes> m_vecPrivRes;
	
private:
	static CFileMonitor* pFileMonitor;
};

#endif