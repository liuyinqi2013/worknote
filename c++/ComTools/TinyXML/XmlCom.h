#ifndef _H_H_XMLCOM
#define _H_H_XMLCOM

#include <string>

class TiXmlNode;
class TiXmlDocument;
class TiXmlElement;
class TiXmlComment;
class TiXmlUnknown;
class TiXmlAttribute;
class TiXmlText;
class TiXmlDeclaration;

namespace CXmlCom
{		
	std::string GetAttribtue(const TiXmlElement* pElement, const std::string& attrName);

	int  GetAttribtueInt(const TiXmlElement* pElement, const std::string& attrName);

	void SetAttribtue(TiXmlElement* pElement, 
		const std::string& attrName,
		const std::string& attrValue);

	void SetAttribtue(TiXmlElement* pElement, 
		const std::string& attrName,
		const int& attrValue);

	std::string GetText(const TiXmlElement* pElement);

	void SetText(TiXmlElement* pElement, const std::string& text);

	TiXmlText* GetFirstChildText(const TiXmlElement* pElement);

	TiXmlNode* GetFirstChildNode(const TiXmlNode* pNode);

	TiXmlNode* GetFirstChildNode(const TiXmlNode* pNode, const std::string& name);

	TiXmlElement* GetFirstChildElement(const TiXmlNode* pNode); 

	TiXmlElement* GetFirstChildElement(const TiXmlNode* pNode, const std::string& name);

	TiXmlElement* GetChildElement(const TiXmlNode* pNode, int index); 

	TiXmlElement* GetChildElement(const TiXmlNode* pNode, const std::string& name, int index); 

	std::string GetTagName(TiXmlElement* pElement);

	void SetTagName(TiXmlElement* pElement, std::string tagName);

	int GetChildElementCount(TiXmlNode* pNode);

	int GetChildElementCount(TiXmlNode* pNode, const std::string& name);

	int GetBrotherElementCount(TiXmlNode* pNode);

	int GetBrotherElementCount(TiXmlNode* pNode, const std::string& name);

	std::string ToString(TiXmlNode* pNode);

	void AddChildNode(TiXmlNode* pParentNode, TiXmlNode* pNewNode);
}

#endif