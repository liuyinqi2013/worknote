#include "MySQLDB.h"
#include "cts.h"
#include "StringTools.h"
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>

CMySQLDB gMySQLWrite;
CMySQLDB gMySQLRead;

void Write()
{
	std::string sqls = CT::GetFileContent("write.sql");
	CT::Trimmed(sqls);
	cout<<"sqls = "<<sqls<<endl;
	std::vector<std::string> vecSQL;
	CT::CutString(sqls, vecSQL, ";");
	gMySQLWrite.Begin();

	for(int i = 0; i < vecSQL.size(); ++i)
	{
		CT::Trimmed(vecSQL[i]);
		cout<<"SQL = "<<vecSQL[i]<<endl;
		if(gMySQLWrite.Excute(vecSQL[i].c_str()))
		{
			cout<<"Write:Exctue ok"<<endl;		
		}
		else
		{	gMySQLWrite.Rollback();
			cout<<"Write error:"<<gMySQLWrite.GetLastErr()<<endl;
			return ;
		}
		usleep(1000);
	}
	
	gMySQLWrite.Commit();
}

void Read()
{
	std::string sqls = CT::GetFileContent("read.sql");
	CT::Trimmed(sqls);
	cout<<"sqls = "<<sqls<<endl;
	if(!gMySQLRead.Query(sqls.c_str()))
	{
		cout<<"Read:Query fail"<<endl;
		cout<<"Read error:"<<gMySQLRead.GetLastErr()<<endl;
		return;
	}

	std::vector<std::string> vecFields;
	CStrMapArr vecValues;
	gMySQLRead.GetFields(vecFields);
	gMySQLRead.GetValues(vecValues);
	
	for(int i = 0; i < vecFields.size(); ++i)
	{
		cout<<vecFields[i]<<"\t";
	}
	cout<<endl;
	
	for(int j = 0; j < vecValues.size(); j++)
	{
		for(int i = 0; i < vecFields.size(); ++i)
		{
			cout<<vecValues[j][vecFields[i]]<<"\t";
		}
		cout<<endl;
	}
}

int main(int argc, char** argv)
{
	cout<<"["<<CMySQLDB::EscapeString("'\"laoda\n\"\'\t")<<"]"<<endl;
	
	if (argc < 2)
	{
		cout<<"Usage : test_cmd [-r|-w]"<<endl;
		return -1;
	}
	
	if(gMySQLWrite.Connect("192.168.11.167", 3306, "", "liuyq", "123456"))
	{
		cout<<"mysql 192.168.11.167 connect ok"<<endl;
	}
	else
	{
		cout<<"mysql 192.168.11.167 connect failed"<<endl;
		return -1;
	}

	if(gMySQLRead.Connect("192.168.11.41", 3306, "", "liuyq", "Liuyq@123456"))
	{
		cout<<"mysql 192.168.11.41 connect ok"<<endl;
	}
	else
	{
		cout<<"mysql 192.168.11.41 connect failed"<<endl;
		return -1;
	}

	cout<<"["<<gMySQLRead.SelectDB("test")<<"]"<<endl;
	cout<<"["<<gMySQLRead.SelectDB("wow")<<"]"<<endl;
	if (argv[1][0] == '-')
	{
		switch(argv[1][1])
		{
		case 'r':
			Read();
			break;
		case 'w':
			Write();
			break;
		default:
			Read();
		}		
	}
	
	return 0;
}
