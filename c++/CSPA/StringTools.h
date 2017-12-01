#ifndef _H_STRING_TOOLS_H
#define _H_STRING_TOOLS_H

#include <vector>
#include <string>
using namespace std;

#define IsSpace(x) ((x) == '\t' || (x) == ' ' || (x) == '\n' || (x) == '\f' || (x) == '\v' || (x) == '\b')
#define BLANK_SET " \t\n\f\v\b"

namespace CT
{
	std::string TrimLeft(std::string& strInOut);

	std::string TrimRight(std::string& strInOut);

	std::string Trimmed(std::string& strInOut);

	std::string ToUpper(std::string& strInOut);

	std::string ToLower(std::string& strInOut);

	std::string RemoveFlag(std::string& strInOut, const string& strFlag = " ");

	std::string RemoveFlagOnce(std::string& strInOut, const string& strFlag = " ");

	std::string Replace(std::string& strInOut, const std::string& subSrc, const std::string& subDst);

	std::string ReplaceOnce(std::string& strInOut, const std::string& subSrc, const std::string& subDst);

	bool StartsWith(const std::string& strIn, const std::string& substr, bool bCase = true);

	bool EndsWith(const std::string& strIn, const std::string& substr, bool bCase = true);
		
	void CutString(string strInput, 
		vector<string>& vecOutput, 
		const string flag = " ", 
		bool bjumpSpace = true);

	void CutStringFirstOf(string strInput, 
		vector<string>& vecOutput, 
		const string flagSet = BLANK_SET, 
		bool bjumpSpace = true);

	std::string EncodeHex(const std::string& strIn, bool bUp = false, bool bPrefix = false);

	std::string DecodeHex(const std::string& strIn);

	std::string EncodeBase64(const std::string& strIn);
	
	std::string DecodeBase64(const std::string& strIn);
}

#endif