#include <stdio.h>
#include "StringFunc.h"

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