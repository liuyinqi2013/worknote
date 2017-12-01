﻿#include "MySQLDB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MS2C(str) ((str).c_str())
#define MC2S(ptr) ((!(ptr))?string(""):string((ptr)))

bool CMySQLDB::Connect()
{
    DisConnect();

	m_DBHandle = mysql_init(NULL);
	if (!m_DBHandle)
	{
		return false;
	}

	char value = 1;
	if (mysql_options(m_DBHandle, MYSQL_OPT_RECONNECT, (char *)&value))
	{
		return false;
	}

	mysql_options(m_DBHandle, MYSQL_SET_CHARSET_NAME, "utf8");
	if (!mysql_real_connect(m_DBHandle, m_Host.c_str(), m_UserName.c_str(), m_PassWord.c_str(), m_DBName.c_str(), m_Port, NULL, 0))
	{
		return false;
	}

	mysql_query(m_DBHandle, "SET NAMES UTF8");
    m_IsConnect = true;
	m_DBStmt =  mysql_stmt_init(m_DBHandle);
	return true;
}


MYSQL_RES* CMySQLDB::Query(const char* sql)
{
	int res = mysql_query(m_DBHandle, sql);	
	if(res != 0)
	{
		return NULL;
	}

	return mysql_store_result(m_DBHandle);
}

int CMySQLDB::StmtPrepare(const char* sql)
{
	int iRet = 0;
	iRet = mysql_stmt_prepare(m_DBStmt, sql, strlen(sql));
	return iRet;
}

int CMySQLDB::StmtBindParamInt(int iparamNum, int iVal)
{
	MYSQL_BIND buf = {0};
	memset(&buf, 0, sizeof(buf));
	buf.buffer = (void*)&iVal;
	buf.buffer_length = sizeof(int);
	buf.buffer_type = MYSQL_TYPE_LONG;
	buf.is_null = 0;
	int iRet = 0;
	iRet = mysql_stmt_bind_param(m_DBStmt, &buf);
	return iRet;
}

int CMySQLDB::StmtBindParamStr(int iparamNum, const string& strVal)
{
	MYSQL_BIND buf = {0};
	memset(&buf, 0, sizeof(buf));
	buf.buffer = (void*)strVal.c_str();
	buf.buffer_length = strVal.length();
	buf.buffer_type = MYSQL_TYPE_STRING;
	buf.is_null = 0;
	int iRet = 0;
	iRet = mysql_stmt_bind_param(m_DBStmt, &buf);
	return iRet;
}

int CMySQLDB::StmtBind(MYSQL_BIND* bind)
{
	int iRet = 0;
	iRet = mysql_stmt_bind_param(m_DBStmt, bind);
	return iRet;
}

int CMySQLDB::StmtExecute()
{
	int iRet = 0;
	iRet = mysql_stmt_execute(m_DBStmt);
	return iRet;
}

string CMySQLDB::StmtSQLStatus()
{
	return string(mysql_stmt_sqlstate(m_DBStmt));
}

int CMySQLDB::GetTabRowCount(const string& tableName)
{
    string SQL = "SELECT COUNT(*) FROM " + tableName;
    return GetRowCount(SQL);
}

int CMySQLDB::GetRowCount(const string& SQL)
{
	MYSQL_RES* result = Query(SQL.c_str());
    if(NULL == result)
    {
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int ret = 0;
    if(row && row[0])
    {
        ret = atoi(row[0]);
    }
    mysql_free_result(result);

    return ret;
}

int CMySQLDB::ClearTable(const string& tableName)
{
	std::string SQL = "DELETE FROM " + tableName;
	return (Excute(SQL.c_str()) ? 0 : -1);
}


int CMySQLDB::Export(const string& tableName, string& strOutSQL, int rowOffset, int rowCount)
{
	char SQL[128] = {0};
	if(rowCount > 0)
	{
		sprintf(SQL, "SELECT * FROM %s LIMIT %d,%d", tableName.c_str(), rowOffset, rowCount);
	}
	else
	{
		sprintf(SQL, "SELECT * FROM %s LIMIT %d,%d", tableName.c_str(), rowOffset, GetTabRowCount(tableName) - rowOffset);
	}

	MYSQL_RES* Res = Query(SQL);
	if(!Res)
	{
		return -1;
	}

	vector<string> vecSQL;
	ConvertInsertSQL(Res, vecSQL);
	for(int i = 0; i < vecSQL.size(); ++i)
	{
		strOutSQL += vecSQL[i] + "\n";
	}

	mysql_free_result(Res);

	return 0;
}

bool  CMySQLDB::Excute(const char* sql)
{
	int res = mysql_query(m_DBHandle, sql);	
	if(res != 0)
	{
		return false;
	}

	return true;
}

string CMySQLDB::GetLastErr()
{
	char buf[512] = {0};
#ifndef WIN32
    snprintf(buf, sizeof(buf) - 1, "Error code : %d msg : %s\n", mysql_errno(m_DBHandle), mysql_error(m_DBHandle));
#else
	sprintf(buf, "Error code : %d msg : %s\n", mysql_errno(m_DBHandle), mysql_error(m_DBHandle));
#endif
	return buf;
}

MYSQL* CMySQLDB::GetMySQLHandle() const 
{
	return m_DBHandle;
}

void CMySQLDB::FreeResult(MYSQL_RES* result)
{
	mysql_free_result(result);
}

bool CMySQLDB::IsStringType(int columnType)
{
	bool b = true;
	switch (columnType)
	{
	case MYSQL_TYPE_SHORT:
	case MYSQL_TYPE_LONG:
	case MYSQL_TYPE_FLOAT:
	case MYSQL_TYPE_INT24:
	case MYSQL_TYPE_NULL:
	case MYSQL_TYPE_LONGLONG:
	case MYSQL_TYPE_BIT:
	case MYSQL_TYPE_ENUM:
	case MYSQL_TYPE_SET:
		b = false;
		break;
	default:
		break;
	}

	return b;
}

void CMySQLDB::ConvertInsertSQL(MYSQL_RES* result, std::vector<std::string>& outSQLVec)
{
	if(!result) return;
	int rowNum = mysql_num_rows(result);
	int colNum = mysql_num_fields(result);

	if( 0 == rowNum || 0 == colNum)
	{
		return ;
	}

	MYSQL_ROW row(NULL);
	MYSQL_FIELD* fileds = mysql_fetch_field(result);
	if(!fileds || !fileds[0].table) 
	{
		return;
	}

	std::string tableName = fileds[0].table;
	std::string colNameList;
	std::string valueList;

	std::string SQL;
	for(int i = 0; i < rowNum; ++i)
	{
		colNameList.clear();
		valueList.clear();

		row = mysql_fetch_row(result);
		if(!row) continue;

		for(int k = 0; k < colNum; ++k)
		{
			if(!row[k]) continue;

			if(k)
			{
				colNameList += ",";
				valueList += ",";
			}

			colNameList += fileds[k].org_name;
			valueList += IsStringType(fileds[k].type) ? AddFlag(row[k]) : MC2S(row[k]);
		}

		SQL = "INSERT INTO " + tableName + "(" + colNameList + ") VALUES(" + valueList + ");";
		outSQLVec.push_back(SQL);
	}
}

void CMySQLDB::ConvertUpdateSQL(MYSQL_RES* result, std::vector<std::string>& outSQLVec)
{
	if(!result) return;
	int rowNum = mysql_num_rows(result);
	int colNum = mysql_num_fields(result);

	if( 0 == rowNum || colNum < 2)
	{
		return ;
	}

	MYSQL_ROW row(NULL);
	MYSQL_FIELD* fileds = mysql_fetch_field(result);
	if(!fileds || !fileds[0].table) 
	{
		return;
	}

	std::string tableName = fileds[0].table;
	std::string colNameList;
	std::string strWhere;

	std::string SQL;
	for(int i = 0; i < rowNum; ++i)
	{
		colNameList.clear();
		row = mysql_fetch_row(result);
		if(!row) continue;

		for(int k = 1; k < colNum; ++k)
		{
			if(!row[k]) continue;

			if(k != 1)
			{
				colNameList += ",";
			}

			colNameList += std::string(fileds[k].org_name) + " = " + (IsStringType(fileds[k].type) ? AddFlag(row[k]) : MC2S(row[k]));
		}
		strWhere = std::string(fileds[0].org_name) + " = " + (IsStringType(fileds[0].type) ? AddFlag(row[0]) : MC2S(row[0]));
		SQL = "UPDATE " + tableName + " SET " + colNameList + " WHERE " + strWhere + ";";
		outSQLVec.push_back(SQL);
	}
}

string CMySQLDB::AddFlag(const char* value)
{
	if(!value) return string("''");
	return (string("'") + value + string("'"));
}