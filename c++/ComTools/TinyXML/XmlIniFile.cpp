#include <stdio.h>
#include <stdlib.h>

#include "XmlIniFile.h"

CXmlIniFile::CXmlIniFile(const char* fileName) :
	m_pXmlRoot(NULL),
	m_xmlDoc(fileName),
	m_bChang(false)
{
	TiXmlDeclaration* pdecl = new TiXmlDeclaration("1.0", "UTF-8", "yes");
	m_xmlDoc.LinkEndChild(pdecl);
	if(m_xmlDoc.LoadFile())
	{
		m_pXmlRoot = m_xmlDoc.RootElement();
	}
	else
	{
		m_pXmlRoot = new TiXmlElement("conf");
		m_xmlDoc.LinkEndChild(m_pXmlRoot);
	}

}

CXmlIniFile::~CXmlIniFile()
{
	if(m_bChang)
	{
		Save();
	}
}

int CXmlIniFile::ReadInt(const string& key, int& value, int num)
{
	string str;
	if (ReadStr(key, str, num) == -1) return -1;
	value = atoi(str.c_str());
	return 0;
}

int CXmlIniFile::ReadInt(const string& section, const string& key, int& value, int num)
{
	string str;
	if (ReadStr(section, key, str, num) == -1) return -1;
	value = atoi(str.c_str());
	return 0;
}

int CXmlIniFile::Read_Att_Int(const string& section, const string& key, int& value, int num)
{
	string str;
	if (Read_Att_Str(section, key, str, num) == -1) return -1;
	value = atoi(str.c_str());
	return 0;
}

int CXmlIniFile::ReadStr(const string& key, string& value, int num)
{
	return Read(m_pXmlRoot, key, value, num);
}

int CXmlIniFile::ReadStr(const string& section, const string& key, string& value, int num)
{
	return Read(m_pXmlRoot->FirstChildElement(section.c_str()), key, value, num);
}

int CXmlIniFile::Read_Att_Str(const string& section, const string& key, string& value, int num)
{
	return Read_Att(GetChildXmlElement(m_pXmlRoot, section, num), key, value);
}

int CXmlIniFile::Read(TiXmlElement* pElement, const string& key, string& value, int num)
{
	if(!pElement)
	{
		return -1;
	}

	TiXmlElement* pKeyElment(GetChildXmlElement(pElement, key, num));

	if(pKeyElment && pKeyElment->GetText())
	{
		value = pKeyElment->GetText();
	}

	return 0;
}

int CXmlIniFile::Read_Att(TiXmlElement* pElement, const string& key, string& value)
{
	if(!pElement)
	{
		return -1;
	}

	const char* tmp(pElement->Attribute(key.c_str()));
	if(tmp)
	{
		value = tmp;
	}
	else
	{
		value = "";
	}

	return 0;
}

int CXmlIniFile::WriteInt(const string& key, const int& value, int num)
{
	char buf[32] = {0};
	sprintf(buf, "%d", value);
	return WriteStr(key, string(buf), num);
}

int CXmlIniFile::WriteInt(const string& section, const string& key, const int& value, int num )
{
	char buf[32] = {0};
	sprintf(buf, "%d", value);
	return WriteStr(section, key, string(buf), num);
}

int CXmlIniFile::Write_Att_Int(const string& section, const string& key, const int& value, int num)
{
	char buf[32] = {0};
	sprintf(buf, "%d", value);
	return Write_Att_Str(section, key, string(buf), num);
}

int CXmlIniFile::WriteStr(const string& key, const string& value, int num)
{
	return Write(m_pXmlRoot, key, value, num);
}

int CXmlIniFile::WriteStr(const string& section, const string& key, const string& value, int num)
{
	TiXmlElement* pElment = GetChildXmlElement(m_pXmlRoot, section, num);
	if(!pElment)
	{
		pElment = new TiXmlElement(section.c_str());
		m_pXmlRoot->LinkEndChild(pElment);
	}

	return Write(pElment, key, value);
}

int CXmlIniFile::Write(TiXmlElement* pElement, const string& key, const string& value, int num)
{
	if(!pElement)
	{
		return -1;
	}

	TiXmlElement* pKeyElment(GetChildXmlElement(pElement, key, num));
	if(!pKeyElment)
	{
		pKeyElment = new TiXmlElement(key.c_str());
		pElement->LinkEndChild(pKeyElment);
	}

	TiXmlNode* pNode = pKeyElment->FirstChild();
	if (!pNode)
	{
		pNode = new TiXmlText(value.c_str());
		pKeyElment->LinkEndChild(pNode);
	}

	if(pNode->Type() == 4)
	{
		pNode->SetValue(value.c_str());
	}

	m_bChang = true;

	return 0;
}

int CXmlIniFile::Write_Att_Str(const string& section, const string& key, const string& value, int num)
{
	TiXmlElement* pElment = GetChildXmlElement(m_pXmlRoot, section, num);
	if(!pElment)
	{
		pElment = new TiXmlElement(section.c_str());
		m_pXmlRoot->LinkEndChild(pElment);
	}

	return Write_Att(pElment, key, value);
}

int CXmlIniFile::Write_Att(TiXmlElement* pElement, const string& key, const string& value)
{
	if(!pElement)
	{
		return -1;
	}

	pElement->SetAttribute(key.c_str(), value.c_str());
	m_bChang = true;
	return 0;
}

int CXmlIniFile::GetKeyCount(const string& key)
{
	return GetChildKeyCount(m_pXmlRoot, key);
}

int CXmlIniFile::GetKeyCount(const string& section, const string& key, int num)
{
	return GetChildKeyCount(GetChildXmlElement(m_pXmlRoot, section, num), key);
}

int CXmlIniFile::GetChildKeyCount(TiXmlElement* pElement, const string& key)
{
	if(!pElement)
	{
		return -1;
	}

	int  n = 0;

	TiXmlElement* pKeyElment(pElement->FirstChildElement(key.c_str()));
	while(pKeyElment)
	{
		++n;
		pKeyElment = pKeyElment->NextSiblingElement(key.c_str());
	}

	return n;
}      


TiXmlElement* CXmlIniFile::GetChildXmlElement(TiXmlElement* pElement, const string& key, int num)
{
	if(!pElement)
	{
		return NULL;
	}

	TiXmlElement* pKeyElment(pElement->FirstChildElement(key.c_str()));
	int i = 0;
	for(; pKeyElment && i < num; ++i)
	{
		pKeyElment = pKeyElment->NextSiblingElement(key.c_str());
	}

	if(i < num)
	{
		return NULL;
	}

	return pKeyElment;
}

bool CXmlIniFile::Save()
{
	return m_xmlDoc.SaveFile();
}

void CXmlIniFile::SetRootName(const string& name)
{
	m_pXmlRoot->SetValue(name.c_str());
}