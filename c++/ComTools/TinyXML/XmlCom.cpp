#include "XmlCom.h"
#include "tinyxml.h"

namespace CXmlCom
{	
	std::string GetAttribtue(const TiXmlElement* pElement, const std::string& attrName)
	{
		std::string attr("");
		if(pElement)
		{
			attr = pElement->Attribute(attrName.c_str());
		}

		return attr;
	}

	int GetAttribtueInt(const TiXmlElement* pElement, const std::string& attrName)
	{
		int attr(0);
		if(pElement)
		{
			pElement->Attribute(attrName.c_str(), &attr);
		}

		return attr;
	}

	void SetAttribtue(TiXmlElement* pElement, 
		const std::string& attrName,
		const std::string& attrValue)
	{
		if(pElement)
		{
			pElement->SetAttribute(attrName.c_str(), attrValue.c_str());
		}
	}

	void SetAttribtue(TiXmlElement* pElement, 
		const std::string& attrName,
		const int& attrValue)
	{
		if(pElement)
		{
			pElement->SetAttribute(attrName.c_str(), attrValue);
		}
	}

	std::string GetText(const TiXmlElement* pElement)
	{
		std::string text("");
		if(pElement && pElement->GetText())
		{
			text = pElement->GetText();
		}
		return text;
	}

	TiXmlNode* GetFirstChildNode(const TiXmlNode* pNode)
	{
		return pNode ? (TiXmlNode*)pNode->FirstChild() : NULL;
	}

	TiXmlNode* GetFirstChildNode(const TiXmlNode* pNode, const std::string& name)
	{
		return pNode ? (TiXmlNode*)pNode->FirstChild(name.c_str()) : NULL;
	}

	TiXmlElement* GetFirstChildElement(const TiXmlNode* pNode)
	{
		return pNode ? (TiXmlElement*)pNode->FirstChildElement() : NULL;
	}

	TiXmlElement* GetFirstChildElement(const TiXmlNode* pNode, const std::string& name)
	{
		return pNode ? (TiXmlElement*)pNode->FirstChildElement(name.c_str()) : NULL;
	}

	TiXmlElement* GetChildElement(const TiXmlNode* pNode, int index)
	{
		TiXmlHandle handle((TiXmlNode*)pNode);
		return handle.ChildElement(index).ToElement();
	}

	TiXmlElement* GetChildElement(const TiXmlNode* pNode, const std::string& name, int index)
	{
		TiXmlHandle handle((TiXmlNode*)pNode);
		return handle.ChildElement(name.c_str(), index).ToElement();
	}

	TiXmlText* GetFirstText(const TiXmlElement* pElement)
	{
		TiXmlNode* pNode(NULL);
		TiXmlText* pTextNode(NULL);
		if(pElement)
		{
			pNode = (TiXmlNode*)pElement->FirstChild();
			while(pNode && TiXmlNode::TINYXML_TEXT != pNode->Type()) pNode = pNode->NextSibling();
			if(pNode) pTextNode =  pNode->ToText();
		}
		return pTextNode;
	}

	void SetText(TiXmlElement* pElement, const std::string& text)
	{
		if(pElement)
		{
			TiXmlText* pTextNode = GetFirstText(pElement);
			if(!pTextNode)
			{
				pTextNode =  new TiXmlText(text.c_str());
				pElement->LinkEndChild(pTextNode);
			}
			else 
			{
				pTextNode->SetValue(text.c_str());
			}
		}
	}

	std::string GetTagName(TiXmlElement* pElement)
	{
		std::string name("");
		if(pElement)
		{
			name =  pElement->Value();
		}
		
		return name;
	}

	void SetTagName(TiXmlElement* pElement, std::string tagName)
	{
		if(pElement)
		{
			pElement->SetValue(tagName.c_str());
		}
	}

	int GetChildElementCount(TiXmlNode* pNode)
	{
		if(NULL == pNode)
		{
			return -1;
		}
		
		int count(0);
		TiXmlElement* pElem = pNode->FirstChildElement();
		while(pElem)
		{ 
			++count;
			pElem = pElem->NextSiblingElement();
		}

		return count;
	}

	int GetChildElementCount(TiXmlNode* pNode, const std::string& name)
	{
		if(NULL == pNode)
		{
			return -1;
		}
		
		int count(0);
		TiXmlElement* pElem = pNode->FirstChildElement(name.c_str());
		while(pElem)
		{ 
			++count;
			pElem = pElem->NextSiblingElement(name.c_str());
		}

		return count;
	}

	int GetBrotherElementCount(TiXmlNode* pNode)
	{
		if(NULL == pNode)
		{
			return -1;
		}

		return GetChildElementCount(pNode->Parent());
	}

	int GetBrotherElementCount(TiXmlNode* pNode, const std::string& name)
	{
				if(NULL == pNode)
		{
			return -1;
		}

		return GetChildElementCount(pNode->Parent(), name.c_str());
	}

	std::string ToString(TiXmlNode* pNode)
	{
		std::string str("");
		if(pNode)
		{
			TiXmlPrinter printer;
			pNode->Accept(&printer);
			str = printer.CStr() ? printer.CStr() : "";
		}
		return str;
	}

	void AddChildNode(TiXmlNode* pParentNode, TiXmlNode* pNewNode)
	{
		if(pParentNode) pParentNode->LinkEndChild(pNewNode);
	}
}

