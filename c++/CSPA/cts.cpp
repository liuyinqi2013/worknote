#include "cts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

namespace CT
{
	std::string Int2Str(int val)
	{
		char buf[128] = {0};
#ifdef WIN32
		_snprintf(buf, 127, "%d", val);
#else
		snprintf(buf, 127, "%d", val);
#endif
		return C2S(buf);
	}

	int Str2Int(const std::string& val)
	{
		return atoi(S2C(val));
	}

	std::string Double2Str(double val, int d)
	{
		char buf[128] = {0};
		std::string strFmt("%lf");
		if(d != -1) 
		{
			strFmt = "%." + I2S(d) + "lf";
		}
#ifdef WIN32
		_snprintf(buf, 127, S2C(strFmt), val);
#else
		snprintf(buf, 127, S2C(strFmt), val);
#endif
		return C2S(buf);
	}

	double Str2Double(const std::string& val)
	{
		return atof(S2C(val));
	}

	std::string CurrentDateTime(TimeFmt fmt)
	{
		time_t t;
		time(&t);
		struct tm* st = localtime(&t);
		char buf[64] = {0};
		switch (fmt)
		{
		case TFMT_0:
			sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d",  
				st->tm_year + 1900,
				st->tm_mon + 1,
				st->tm_mday,
				st->tm_hour,
				st->tm_min,
				st->tm_sec);
			break;
		case TFMT_1:
			sprintf(buf, "%4d/%02d/%02d %02d:%02d:%02d",  
				st->tm_year + 1900,
				st->tm_mon + 1,
				st->tm_mday,
				st->tm_hour,
				st->tm_min,
				st->tm_sec);
			break;
		case TFMT_2:
			sprintf(buf, "%4d%02d%02d%02d%02d%02d",  
				st->tm_year + 1900,
				st->tm_mon + 1,
				st->tm_mday,
				st->tm_hour,
				st->tm_min,
				st->tm_sec);
			break;
		default:
			sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d",  
				st->tm_year + 1900,
				st->tm_mon + 1,
				st->tm_mday,
				st->tm_hour,
				st->tm_min,
				st->tm_sec);
			break;
		}

		return C2S(buf);
	}

	std::string GetFileName(const std::string& filePathName)
	{
		std::string fileName;
		int pos = filePathName.find_last_of("/\\");
		if(pos != filePathName.npos && pos + 1 != filePathName.size() - 1)
		{
			fileName = filePathName.substr(pos + 1);
		}

		return fileName;
	}

	
	std::string GetFileContent(const std::string& filePathName)
	{
		FILE* fd = fopen(filePathName.c_str(), "rb");
		if(!fd) return std::string("");

		fseek(fd, 0, SEEK_END);
		long size = ftell(fd);
		fseek(fd, 0, SEEK_SET);
		
		char* buf = new char[size + 1];
		if(!buf) return std::string("");
		memset(buf, 0, size + 1);
		
		fread(buf, 1, size, fd);

		string content(buf, size);
		fclose(fd);
		delete[] buf;
		
		return content;
	}
}
