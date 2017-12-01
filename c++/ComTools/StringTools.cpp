#include "StringTools.h"
#include <ctype.h>

namespace CT
{
	std::string TrimLeft(std::string& strInOut)
	{
		int pos = strInOut.find_first_not_of(BLANK_SET);
		if(pos != strInOut.npos)
		{
			strInOut = strInOut.substr(pos);
		}
		return strInOut;
	}

	std::string TrimRight(std::string& strInOut)
	{
		int pos = strInOut.find_last_not_of(BLANK_SET);
		if(pos != strInOut.npos)
		{
			strInOut = strInOut.substr(0, pos + 1);
		}
		return strInOut;
	}

	std::string Trimmed(std::string& strInOut)
	{
		TrimLeft(strInOut);
		return TrimRight(strInOut);
	}

	std::string ToUpper(std::string& strInOut)
	{
		for(int i = 0; i < strInOut.size(); ++i)
		{
			strInOut[i] = toupper(strInOut[i]);
		}
		return strInOut;
	}

	std::string ToLower(std::string& strInOut)
	{
		for(int i = 0; i < strInOut.size(); ++i)
		{
			strInOut[i] = tolower(strInOut[i]);
		}
		return strInOut;
	}

	std::string RemoveFlag(std::string& strInOut, const string& strFlag)
	{
		int pos =  strInOut.find(strFlag);
		while(pos != strInOut.npos)
		{
			strInOut.erase(pos, strFlag.size());
			pos = strInOut.find(strFlag, pos);
		}
		return strInOut;
	}

	std::string RemoveFlagOnce(std::string& strInOut, const string& strFlag)
	{
		int pos  = strInOut.find(strFlag);
		if(pos != strInOut.npos)
		{
			strInOut.erase(pos, strFlag.size());
		}
		return strInOut;
	}

	std::string Replace(std::string& strInOut, const std::string& subSrc, const std::string& subDst)
	{
		int pos = strInOut.find(subSrc);
		while(pos != strInOut.npos)
		{
			strInOut.replace(pos, subSrc.size(), subDst);
			pos = strInOut.find(subSrc, pos + subDst.size());
		}
		return strInOut;
	}

	std::string ReplaceOnce(std::string& strInOut, const std::string& subSrc, const std::string& subDst)
	{
		int pos = strInOut.find(subSrc);
		if(pos != strInOut.npos)
		{
			strInOut.replace(pos, subSrc.size(), subDst);
		}
		return strInOut;
	}

	bool StartsWith(const std::string& strIn, const std::string& substr, bool bCase)
	{
		bool bRet = true;
		if(bCase)
		{
			for(int i = 0; i < strIn.size() && i < substr.size(); i++)
			{
				if(strIn[i] != substr[i]){
					bRet = false;
					break;
				}
			}
		}
		else
		{
			for(int i = 0; i < strIn.size() && i < substr.size(); i++)
			{
				if(strIn[i] != substr[i]){
					bRet = false;
					break;
				}
			}
		}

		return bRet;
	}

	bool EndsWith(const std::string& strIn, const std::string& substr, bool bCase)
	{
		bool bRet = true;
		if(bCase)
		{
			for(int i = 0; i < strIn.size() && i < substr.size(); i++)
			{
				if(strIn[i] != substr[i]){
					bRet = false;
					break;
				}
			}
		}
		else
		{
			for(int i = 0; i < strIn.size() && i < substr.size(); i++)
			{
				if(strIn[i] != substr[i]){
					bRet = false;
					break;
				}
			}
		}

		return bRet;
	}

	void CutString(string strInput, vector<string>& vecOutput, const string flag, bool bjumpSpace)
	{
		vecOutput.clear();
		int begin = 0;
		int end = 0;

		strInput += flag;

		while(begin < strInput.size() && (end = strInput.find(flag, begin)) != string::npos)
		{
			if(bjumpSpace && end <= begin)
			{
				begin = end + flag.size();
				continue;
			}

			vecOutput.push_back(strInput.substr(begin, end - begin));
			begin = end + flag.size();
		}
	}

	void CutStringFirstOf(string strInput, vector<string>& vecOutput, const string flagSet, bool bjumpSpace)
	{
		vecOutput.clear();
		int begin = 0;
		int end = 0;

		strInput += flagSet[0];

		while(begin < strInput.size() && (end = strInput.find_first_of(flagSet, begin)) != string::npos)
		{
			if(bjumpSpace && end <= begin)
			{
				begin = end + 1;
				continue;
			}

			vecOutput.push_back(strInput.substr(begin, end - begin));
			begin = end + 1;
		}
	}

}