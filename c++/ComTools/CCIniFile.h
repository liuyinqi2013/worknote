#ifndef H_CCINIFILE_LIUYQ_20150204_H
#define H_CCINIFILE_LIUYQ_20150204_H
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
namespace inifile {

	enum {
		ININOTE,
		INIKEY,
		INISECTION,
	};

	inline bool IsSpace(int p) {
		return p == '\r' || p == '\n' || p == ' ' || p == '\t' || p == '\v' || p == '\f';
	}

	class CCNode;
	class CCNoteNode;
	class CCKeyNode;
	class CCSectionNode;
	class CCIniElement;
	class CCIniDocument;
	ostream& operator<<(ostream& os, CCIniDocument& obj);

	class CCNode {
		friend class CCIniElement;
	protected:
		CCNode(const string& Name, 
			const string& Value,
			int Type):use(1),
			type(Type), name(Name), value(Value) {}
		virtual ~CCNode(){}
		virtual void Print(ostream &os) const = 0;
		void SetName(const string &Name) { name = Name;}
		void SetValue(const string &Value) { value = Value; }
		const string& GetName() const { return name; }
		const string& GetValue() const { return value; }
		int GetType() const { return type;}
	protected:
		int use;
		int type;
		string name;
		string value;
	};

	class CCNoteNode : public  CCNode{
	private:
		friend class CCIniElement;
		CCNoteNode(const string& Value) : CCNode("#", Value, ININOTE){}
		virtual ~CCNoteNode() {};
		virtual void Print(ostream &os) const {
			os<<"#"<<value<<endl;
		}
	};

	class CCKeyNode : public  CCNode{
	private:
		friend class CCIniElement;
		CCKeyNode(const string& Name, const string Value) : CCNode(Name, Value, INIKEY){}
		virtual ~CCKeyNode() {}
		virtual void Print(ostream &os) const {
			os<<name<<"="<<value<<endl;
		}
	};

	class CCSectionNode : public  CCNode{
	private:
		friend class CCIniElement;
		CCSectionNode(const string& Name, int Index) : CCNode(Name, "", INISECTION), index(Index) {}
		virtual ~CCSectionNode(){};
		virtual void Print(ostream &os) const {
			os<<"["<<name<<"]"<<endl;
		}
	private:
		int index;
	};

	class CCIniElement {
	public:
		CCIniElement(const string& Value) : p(new CCNoteNode(Value)){}
		CCIniElement(const string& Name, int Index) : p(new CCSectionNode(Name, Index)){}
		CCIniElement(const string& Name, const string Value) : p(new CCKeyNode(Name, Value)){}
		CCIniElement(const CCIniElement& rhs) {
			p = rhs.p;
			++p->use;
		}
		CCIniElement& operator=(const CCIniElement& rhs) {
			++rhs.p->use;
			if (--p->use == 0)
				delete p;
			p = rhs.p;
			return *this;
		}
		~CCIniElement() {
			if (--p->use == 0)
				delete p;
		}
		void SetName(const string &Name) {
			p->SetName(Name);
		}
		void SetValue(const string &Value) {
			p->SetValue(Value);
		}
		const string& GetName() const {
			return p->GetName();
		}
		const string& GetValue() const {
			return p->GetValue();
		}
		int GetType() const {
			return p->GetType();
		}
		void Print(ostream &os) const {
			p->Print(os);
		}
	private:
		CCNode *p;
	};

	 typedef struct CCIniElementList {
		 CCIniElementList() : element(NULL),
		 next(NULL), prev(NULL){}
		 CCIniElement *element;
		 CCIniElementList *next;
		 CCIniElementList *prev;
	 };

	class CCIniDocument {
	public:
		friend ostream& operator<<(ostream& os, CCIniDocument& obj);
		CCIniDocument() : number(0), head(NULL), tail(NULL){}
		CCIniDocument(const char *str) : number(0), head(NULL), tail(NULL){
			Parse(str);
		};
		~CCIniDocument();
		void Parse(const char *str);
		bool Load(const char* fileName);
		CCIniElementList* InsertNote(const string& noteValue);
		CCIniElementList* InsertSection(const string& sectionName);
		CCIniElementList* InsertKey(const string& sectionName, const string& key, const string& value);
		void DeleteNote(const string& noteValue);
		void DeleteSection(const string& sectionName);
		void DeleteKey(const string& sectionName, const string& keyName);
		CCIniElementList* GetElementSection(const string& sectionName);
		CCIniElementList* GetElementKey(const string& sectionName, const string& keyName);
		CCIniElementList* GetElementNote(const string& NoteValue);
		bool Save(const char *filename);
		void Print(ostream& os);
	private:
		CCIniElementList* InsertPrev(CCIniElementList *dest, CCIniElementList *src);
		CCIniElementList* InsertNext(CCIniElementList *dest, CCIniElementList *src);
		void Delete(CCIniElementList *root);
	private:
		int number;
		CCIniElementList *head;
		CCIniElementList *tail;
	};

	void TestDoc();

	class CCIniFile {
	public:
		CCIniFile(const char *fileName) : filename(fileName) {
			doc.Load(filename);
		}
		void ReadInt(const string& section, const string& key, int& value);
		void ReadFloat(const string& section, const string& key, double& value);
		void ReadLong(const string& section, const string& key, long& value);
		void ReadString(const string& section, const string& key, string& value);

		void WriteInt(const string& section, const string& key, int value);
		void WriteFloat(const string& section, const string& key, double value);
		void WriteLong(const string& section, const string& key, long value);
		void WriteString(const string& section, const string& key, string value);
		~CCIniFile(){};
		void Print() {cout<<doc<<endl;}
	private:
		CCIniFile(const CCIniFile&);
		CCIniFile& operator=(const CCIniFile&);
		int Write(const string& section, const string& key, string value);
		int Read(const string& section, const string& key, string& value);
	private:
		const char *filename;
		CCIniDocument doc;
	};
}

#endif