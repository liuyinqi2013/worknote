#ifndef H_XMLPARSER_H
#define H_XMLPARSER_H

#include <cstdlib>
#include <string>
#include "tinyxml.h"

using namespace std;

class CXmlParser
{
public:
	CXmlParser(const char* fileName);  

	~CXmlParser();

	int Read(const string& key, int& value);

	int Read(const string& key, string& value);

	int Read(const string& section, const string& key, int& value);

	int Read(const string& section, const string& key, string& value);

private:
	int Read(TiXmlElement* pElement, const string& key, string& value);
private:
	TiXmlDocument m_xmlDoc;
	TiXmlElement* m_pXmlRoot;
};


#endif 