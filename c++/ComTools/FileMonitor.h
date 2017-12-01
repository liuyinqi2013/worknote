#ifndef _H_H_FILEMONITOR
#define _H_H_FILEMONITOR

#include <map>
#include <string>
#include <vector>

#include <pthread.h>

//�ļ���������
#define FILE_CREATE 1   //�����ļ�
#define FILE_DELETE 2   //ɾ���ļ�
#define FILE_MOVE   3   //�Ƶ��ļ�
#define FILE_RENAME 4   //�������ļ�
#define FILE_SAVE   5   //�����ļ�

//�ļ�����

#define FNORMAL 1   //��ͨ�ļ�
#define FDIR    2   //�ļ���

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
		
	int    iOpType;         //�ļ���������
	string strSrcPathName;  //Դ�ļ�·��
	string strDestPathName; //Ŀ���ļ�·��
	string strSrcFileName;  //Դ�ļ���
	string strDestFileName; //Ŀ���ļ���
	string strDesc;
	
	int cooks;
	int iFileType;          //�ļ�����
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

	int Init();   //ʹ��ǰ���ó�ʼ��
	int Uninit(); //ʹ�ú���÷���ʼ�� 
	
	int AddMonitorFilePath(string& filePath);      //���Ӽ���ļ��С��������ļ��С�
	int DelMonitorWatchFilePath(string& filePath); //ɾ����ص��ļ��С�

	int GetFileOperRes(vector<stuFileOperRes>& vecRes); //��ȡ���������û�н����һֱ������

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