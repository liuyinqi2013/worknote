#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "CCIniFile.h"
namespace inifile {

	CCIniDocument::~CCIniDocument() {
		Delete(head);
	}
	bool CCIniDocument::Load(const char* fileName) {
		if (fileName == NULL)
			return false;
		FILE *fd = fopen(fileName, "rt+");
		if (fd == NULL)
			return false;
		fseek(fd, 0, SEEK_END);
		unsigned int len = ftell(fd);
		fseek(fd, 0, SEEK_SET);
		char *buf = new char[len + 1];
		fread(buf, 1, len, fd);
		Parse(buf);
		fclose(fd);
		delete[] buf;
		return true;
	}

	bool CCIniDocument::Save(const char *filename) {
		if (filename == NULL)
			return false;
		ofstream out(filename);
		if (out.bad())
			return false;
		Print(out);
		return true;
	}

	void CCIniDocument::Parse(const char *str) {
		if (str == NULL)
			return ;
		Delete(head);
		const char *p = str;
		const char *p1 = str;
		tail = head = new CCIniElementList;
		CCIniElementList *pCurr = head;
		while (*p != '\0') {
			while (*p1 != '\n' && *p1 != '\0') ++p1;
			while (IsSpace(*p) && p != p1) ++p;
			const char *pend = p1;
			while (IsSpace(*pend) && pend != p) --pend;
			if (p != pend) {
				CCIniElement *pElement;
				CCIniElementList *pList = new CCIniElementList;;	
				if (*p == '#') {
					string value(p + 1, pend + 1);
					pElement = new CCIniElement(value);
				} else if (*p == '[') {
					const char *pTmp = pend;
					while(*pTmp != ']' && pTmp != p) --pTmp;
					if (*pTmp != ']') goto next;
					string name(p + 1, pTmp);
					pElement = new CCIniElement(name, number++);

				} else {
					const char *pTmp = p + 1;
					while(*pTmp != '=' && pTmp != pend) ++pTmp;
					if (*pTmp != '=') goto next;
					string name(p, pTmp);
					string value(pTmp + 1, pend + 1);
					pElement = new CCIniElement(name, value);
				}
				pList->element = pElement;
				pList->prev = pCurr;
				pCurr->next = pList;
				pCurr = pList;
			}
next:
			p = p1;
			p1 = p + 1;
		}
		tail = pCurr;
	}

	CCIniElementList* CCIniDocument::GetElementSection(const string& sectionName) {
		CCIniElementList *p = head->next;
		while(p != NULL) {
			CCIniElement *pElement = p->element;
			if (pElement->GetType() == INISECTION && 
				pElement->GetName() == sectionName)
				return p;
			p = p->next;
		}
		return p;
	}

	CCIniElementList* CCIniDocument::GetElementKey(const string& sectionName, const string& keyName) {
		CCIniElementList* p = GetElementSection(sectionName);
		if (p == NULL)
			return NULL;
		p = p->next;
		while(p != NULL) {
			CCIniElement *pElement = p->element;
			if (pElement->GetType() == INISECTION)
				return NULL;
			if (pElement->GetType() == INIKEY &&
				pElement->GetName() == keyName)
				return p;
			p = p->next;
		}
		return NULL;
	}

	CCIniElementList* CCIniDocument::GetElementNote(const string& NoteValue) {
		CCIniElementList *p = head->next;
		while(p != NULL) {
			CCIniElement *pElement = p->element;
			if (pElement->GetType() == ININOTE && 
				pElement->GetName() == NoteValue)
				return p;
			p = p->next;
		}
		return p;
	}

	void CCIniDocument::DeleteNote(const string& noteValue) {
		CCIniElementList *p = GetElementNote(noteValue);
		if (p == NULL)
			return;
		CCIniElementList *pNext = p->next;
		CCIniElementList *pPrev = p->prev;
		pPrev->next = pNext;
		if(pNext)
			pNext->prev = pPrev;
		else
			tail = pPrev;
		delete p->element;
		delete p;
	}

	void CCIniDocument::DeleteSection(const string& sectionName) {
		CCIniElementList *p = GetElementSection(sectionName);
		if (p == NULL)
			return;
		CCIniElementList *pPrev = p->prev;
		do {
			CCIniElementList *tmp = p;
			p = p->next;
			delete tmp->element;
			delete tmp;	
		} while(p != NULL && p->element->GetType() != INISECTION);
			pPrev->next = p;
			if (p != NULL)
				p->prev = pPrev;
			else
				tail = pPrev;

	}

	void CCIniDocument::DeleteKey(const string& sectionName, const string& keyName) {
		CCIniElementList *p = GetElementKey(sectionName, keyName);
		if (p == NULL)
			return;
		CCIniElementList *pNext = p->next;
		CCIniElementList *pPrev = p->prev;
		pPrev->next = pNext;
		if(pNext)
			pNext->prev = pPrev;
		else
			tail = pPrev;
		delete p->element;
		delete p;
	}

	CCIniElementList* CCIniDocument::InsertPrev(CCIniElementList *dest, CCIniElementList *src) {
		if (dest == NULL || src == NULL)
			return NULL;
		CCIniElementList *pPrev = dest->prev;
		pPrev->next = src;
		src->prev = pPrev;
		src->next = dest;
		dest->prev = src;
		return src;
	}

	CCIniElementList* CCIniDocument::InsertNext(CCIniElementList *dest, CCIniElementList *src) {
		if (dest == NULL || src == NULL)
			return NULL;
		CCIniElementList *pNext = dest->next;
		if(pNext)
			pNext->prev = src;
		else
			tail = src;
		src->next = pNext;
		src->prev = dest;
		dest->next = src;
		return src;
	}

	CCIniElementList* CCIniDocument::InsertSection(const string& sectionName) {
		CCIniElementList *pSection = GetElementSection(sectionName);
		if (pSection == NULL) {
			CCIniElement *element = new CCIniElement(sectionName, number++);
			CCIniElementList *list = new CCIniElementList;
			list->element = element;
			return InsertNext(tail, list);
		}
		return pSection;
	}

	CCIniElementList* CCIniDocument::InsertKey(const string& sectionName, const string& key, const string& value) {
		CCIniElementList *pSection = InsertSection(sectionName);
		CCIniElement *element = new CCIniElement(key, value);
		CCIniElementList *list = new CCIniElementList;
		list->element = element;
		return InsertNext(pSection, list);
	}

	void CCIniDocument::Delete(CCIniElementList *root) {
		if (root ==  NULL)
			return ;
		Delete(root->next);
		root->next = NULL;
		if (root->element) {
			delete root->element;
			 root->element = NULL;
		}
		delete root;	
	}

	void CCIniDocument::Print(ostream& os) {
		CCIniElementList *p = head->next;
		while(p) {
			p->element->Print(os);
			p = p->next;
		}
	}

	int CCIniFile::Read(const string& section, const string& key, string& value) {
		CCIniElementList *p = doc.GetElementKey(section, key);
		if (p == NULL)
			return -1;
		value = p->element->GetValue();
		return 0;
	}

	void CCIniFile::ReadInt(const string& section, const string& key, int& value) {
		string str;
		Read(section, key, str);
		value = atoi(str.c_str());
	}

	void CCIniFile::ReadFloat(const string& section, const string& key, double& value) {
		string str;
		Read(section, key, str);
		value = atof(str.c_str());
	}

	void CCIniFile::ReadLong(const string& section, const string& key, long& value) {
		string str;
		Read(section, key, str);
		value = atol(str.c_str());
	}

	void CCIniFile::ReadString(const string& section, const string& key, string& value) {
		Read(section, key, value);
	}

	int CCIniFile::Write(const string& section, const string& key, string value) {
		CCIniElementList *p = doc.GetElementKey(section, key);
		if (p == NULL) {
			doc.InsertKey(section, key, value);
		}
		else 
			p->element->SetValue(value);
		doc.Save(filename);
		return 0;
	}

	void CCIniFile::WriteInt(const string& section, const string& key, int value) {
		char buf[30] = {0};
		sprintf(buf, "%d", value);
		Write(section, key, string(buf));
	}

	void CCIniFile::WriteFloat(const string& section, const string& key, double value) {
		char buf[30] = {0};
		sprintf(buf, "%lf", value);
		string str = string(buf);
		Write(section, key, str);
	}

	void CCIniFile::WriteLong(const string& section, const string& key, long value) {
		char buf[30] = {0};
		sprintf(buf, "%ld", value);
		Write(section, key, string(buf));
	}

	void CCIniFile::WriteString(const string& section, const string& key, string value) {
		Write(section, key, value);
	}

	ostream& operator<<(ostream& os, CCIniDocument& obj) {
		obj.Print(os);
		return os;
	}

	void TestDoc() {
		CCIniFile ini("config.txt");
		int num;
		double numfloat;
		double soce;
		string str;
		ini.ReadInt("config", "num", num);
		ini.ReadFloat("config", "numfloat", numfloat);
		ini.ReadFloat("CONF", "SOCE", soce);
		cout<<"num = "<<num<<endl;
		cout<<"numfloat = "<<numfloat<<endl;
		cout<<"soce = "<<soce<<endl;
		ini.ReadString("config", "name", str);
		cout<<"name = "<<str<<endl;
		ini.WriteFloat("CONF", "SOCE", 1.467);
		ini.WriteString("config", "name", "laosan");
		ini.WriteString("config", "add", "sichuan");
		ini.Print();
		CCIniFile rini("conf.ini");
		rini.Print();
	}

}
