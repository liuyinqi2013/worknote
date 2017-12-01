#ifndef H_XMLINIFILE_H
#define H_XMLINIFILE_H

#include <string>
#include "tinyxml.h"

using namespace std;

class CXmlIniFile
{
public:
	CXmlIniFile(const char* fileName);
	~CXmlIniFile();

	int ReadInt(const string& key, int& value, int num = 0);
	int ReadInt(const string& section, const string& key, int& value, int num = 0);
	int Read_Att_Int(const string& section, const string& key, int& value, int num = 0);

	int ReadStr(const string& key, string& value, int num = 0);
	int ReadStr(const string& section, const string& key, string& value, int num = 0);
	int Read_Att_Str(const string& section, const string& key, string& value, int num = 0);

	int WriteInt(const string& key, const int& value, int num = 0);
	int WriteInt(const string& section, const string& key, const int& value, int num = 0);
	int Write_Att_Int(const string& section, const string& key, const int& value, int num = 0);

	int WriteStr(const string& key, const string& value, int num = 0);
	int WriteStr(const string& section, const string& key, const string& value, int num = 0);
	int Write_Att_Str(const string& section, const string& key, const string& value, int num = 0);

	int GetKeyCount(const string& key);
	int GetKeyCount(const string& section, const string& key, int num = 0);

	void SetRootName(const string& name);

	bool Save();

private:
	CXmlIniFile(const CXmlIniFile& rhs);
	CXmlIniFile& operator=(const CXmlIniFile& rhs);

	int Read(TiXmlElement* pElement, const string& key, string& value, int num = 0);
	int Read_Att(TiXmlElement* pElement, const string& key, string& value);

	int Write(TiXmlElement* pElement, const string& key, const string& value, int num = 0);
	int Write_Att(TiXmlElement* pElement, const string& key, const string& value);

	int GetChildKeyCount(TiXmlElement* pElement, const string& key);

	TiXmlElement* GetChildXmlElement(TiXmlElement* pElement, const string& key, int num = 0);

private:
	TiXmlDocument m_xmlDoc;
	TiXmlElement* m_pXmlRoot;
	bool m_bChang;

};    

#endif