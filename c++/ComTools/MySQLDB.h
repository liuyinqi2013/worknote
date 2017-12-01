#ifndef __H__H_TCMOPERATORDB_H
#define __H__H_TCMOPERATORDB_H

#include <string>
#include <vector>

#include "mysql.h"

using namespace std;

class CMySQLDB
{
public:

	CMySQLDB() :
		m_Host(""),
		m_Port(0),
		m_DBName(""),
		m_UserName(""),
		m_PassWord(""),
        m_DBHandle(NULL),
		m_DBStmt(NULL),
        m_IsConnect(false)
	{
	}

	CMySQLDB(
		const char* hostName, 
		const int port, 
		const char* DbName,
		const char* userName,
		const char* passWord):
		m_Host(hostName),
		m_Port(port),
		m_DBName(DbName),
		m_UserName(userName),
		m_PassWord(passWord),
        m_DBHandle(NULL),
		m_DBStmt(NULL),
        m_IsConnect(false)

	{
	}

	~CMySQLDB()
	{
        DisConnect();
	}

	bool Connect();

	bool Connect(const char* hostName, 
		const int port, 
		const char* DbName,
		const char* userName,
		const char* passWord)
	{
		m_Host = hostName;
		m_Port = port;
		m_DBName = DbName;
		m_UserName = userName;
		m_PassWord = passWord;
		return Connect();
	}

    bool IsConnect() const
    {
        return m_IsConnect;
    }

	MYSQL_RES* Query(const char* sql);

	int StmtPrepare(const char* sql);

	int StmtBindParamInt(int iparamNum, int iVal);

	int StmtBindParamStr(int iparamNum, const string& strVal);

	int StmtBind(MYSQL_BIND* bind);

	string StmtSQLStatus();

	int StmtExecute();

	int GetTabRowCount(const string& tableName);

	int GetRowCount(const string& SQL);

	int ClearTable(const string& tableName);

	int Export(const string& tableName, string& strOutSQL, int rowOffset = 0, int rowCount = -1);

	bool Excute(const char* sql);

	void DisConnect()
	{
        if(m_IsConnect) {
            mysql_close(m_DBHandle);
            m_IsConnect = false;
        }
	}

	string GetLastErr();

	MYSQL* GetMySQLHandle() const ;

	void FreeResult(MYSQL_RES* result);

	void ConvertInsertSQL(MYSQL_RES* result, std::vector<std::string>& outSQLVec);

	void ConvertUpdateSQL(MYSQL_RES* result, std::vector<std::string>& outSQLVec);

private:
	bool IsStringType(int columnType);

	string AddFlag(const char* value);
	
private:
	string m_Host;
	int    m_Port;
	string m_DBName;
	string m_UserName;
	string m_PassWord;
	MYSQL* m_DBHandle;
	MYSQL_STMT* m_DBStmt; 
	bool   m_IsConnect;
};

#endif
