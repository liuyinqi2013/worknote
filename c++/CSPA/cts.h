#ifndef _H_COMMON_TOOLS_H
#define _H_COMMON_TOOLS_H

#include <string>

namespace CT
{
	std::string Int2Str(int val);
	int Str2Int(const std::string& val);

	std::string Double2Str(double val, int d = -1);
	double Str2Double(const std::string& val);

	enum TimeFmt
	{
		TFMT_0, //yyyy-MM-dd HH:mm:ss
		TFMT_1, //yyyy/MM/dd HH:mm:ss
		TFMT_2  //yyyyMMddHHmmss
	};

	std::string CurrentDateTime(TimeFmt fmt = TFMT_0);

	std::string GetFileName(const std::string& filePathName);

	std::string GetFileContent(const std::string& filePathName);

}

#define C2S(x) (std::string((x)))
#define S2C(x) (((x).c_str()))
#define I2S(x) (CT::Int2Str((x)))
#define S2I(x) (CT::Str2Int((x)))
#define D2S(x) (CT::Double2Str((x)))
#define D2SEx(x,d) (CT::Double2Str((x),(d)))
#define S2D(x) (CT::Str2Double((x)))
#define CTime() (CT::CurrentDateTime())

#endif 