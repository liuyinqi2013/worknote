#include "XmlParser.h"

CXmlParser::CXmlParser(const char* fileName) : 
	m_pXmlRoot(NULL)
{
	if(m_xmlDoc.LoadFile(fileName))
	{
		m_pXmlRoot = m_xmlDoc.RootElement();
	}
}

CXmlParser::~CXmlParser()
{
	m_xmlDoc.Clear();
}

int CXmlParser::Read(const string& key, int& value)
{
	string strValue("");
	Read(key, strValue);
	value = atoi(strValue.c_str());
	return 0;
}

int CXmlParser::Read(const string& key, string& value)
{
	return Read(m_pXmlRoot, key, value);
}

int CXmlParser::Read(const string& section, const string& key, int& value)
{
	string strValue("");
	Read(section, key, strValue);
	value = atoi(strValue.c_str());
	return 0;
}

int CXmlParser::Read(const string& section, const string& key, string& value)
{
	if(m_pXmlRoot)
	{
		return Read(m_pXmlRoot->FirstChildElement(section.c_str()), key, value);
	}

	return -1;
}

int CXmlParser::Read(TiXmlElement* pElement, const string& key, string& value)
{
	if(!pElement)
	{
		return -1;
	}

	TiXmlElement* pKeyElment(pElement->FirstChildElement(key.c_str()));
	if(pKeyElment)
	{
		value = pKeyElment->GetText();
	}
	else
	{
		value = "";
	}

	return 0;
}