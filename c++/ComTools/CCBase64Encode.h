#pragma once
#include <string>
using std::string;

extern string ToBase64Encode(const string& src);

extern unsigned long ToBase64Encode(const char* src, unsigned long len, string &des);

extern string UnBase64Encode(const string& src);

extern unsigned long UnBase64Encode(const char *src, unsigned long length, string &des);