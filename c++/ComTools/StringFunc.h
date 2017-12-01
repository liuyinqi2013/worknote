#ifndef _H_H_STRINGFUNC
#define _H_H_STRINGFUNC

#include <vector>
#include <string>
using namespace std;

#define IsSpace(x) ((x) == '\t' || (x) == ' ' || (x) == '\n' || (x) == '\f' || (x) == '\v' || (x) == '\b')

void CutString(string strInput, vector<string>& vecOutput, const string flag = " ", bool bjumpSpace = true);

void CutStringFirstOf(string strInput, vector<string>& vecOutput, const string flagSet = " \t\n\f\v\b", bool bjumpSpace = true);

#endif