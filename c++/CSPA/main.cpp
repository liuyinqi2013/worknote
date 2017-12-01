#include "cts.h"
#include <iostream>
#include "StringTools.h"
#include <stdio.h>
#include <fstream>

using namespace CT;

int main(int argc, char** argv)
{
	//std::cout<<"Data Time = "<<CTime()<<std::endl;
	//std::cout<<I2S(10)<<std::endl;
	//std::cout<<S2I("789456")<<std::endl;

	//std::cout<<D2S(10)<<std::endl;
	//std::cout<<D2SEx(10.45554, 2)<<std::endl;
	//std::cout<<S2D("789456")<<std::endl;

	//std::cout<<CT::GetFileName("../../laoda/ss.txt")<<std::endl;
	//std::cout<<CT::GetFileName("..\\..\\laoda\\xx.txt")<<std::endl;

	//string str = "G T  laoda   polaoDaylaodal";
	//string str1 = "  laoder \n\t\b";

	//std::cout<<"["<<CT::TrimLeft(str)<<"]"<<std::endl;
	//std::cout<<"["<<CT::TrimRight(str1)<<"]"<<std::endl;
	//std::cout<<"["<<CT::Trimmed(str)<<"]"<<std::endl;
	//std::cout<<"["<<CT::RemoveFlagOnce(str)<<"]"<<std::endl;
	//std::cout<<"["<<CT::RemoveFlag(str, "laoda")<<"]"<<std::endl;

	//std::cout<<"["<<CT::ToLower(str)<<"]"<<std::endl;
	//std::cout<<"["<<CT::ToUpper(str)<<"]"<<std::endl;

	//std::cout<<"["<<str<<"]"<<std::endl;
	//std::cout<<"["<<CT::ReplaceOnce(str, "l", "xx")<<"]"<<std::endl;
	//std::cout<<"["<<str<<"]"<<std::endl;

	//int a = 5, b = 7;
	//std::cout<<"["<<a++ + ++b<<"]"<<std::endl;

	/*
	char* s = "Laoda";

	cout<<"StartsWith lao : "<<StartsWith(s, "lao")<<endl;
	cout<<"StartsWith Da: "<<EndsWith(s, "Da")<<endl;

	string strHex = EncodeHex(s, true, true);
	cout<<"EncodeHex:"<<strHex<<endl;
	string deHex = DecodeHex(strHex);
	cout<<"DecodeHex:"<<deHex<<endl;
	cout<<"size:"<<deHex.size()<<endl;

	char* h = (char*)s;
	while(*h != '\0')
	{
		printf("%02x", (unsigned char)(*h));
		h++;
	}
	printf("\n");
	*/
	if(argc < 3)
	{
		cout<<"Uasge : hex [opt] [infile] [outfile]"<<endl;;
		return -1;
	}

	ifstream infile(argv[2], ios_base::in | ios_base::binary);
	if(!infile.is_open())
	{
		cout<<"open file failed : "<<argv[2]<<endl;;
	}
	
	ofstream outfile(argv[3], ios_base::out | ios_base::binary);
	if(!outfile.is_open())
	{
		cout<<"open file failed : "<<argv[3]<<endl;;
	}

	infile.seekg(0, ios_base::end);
	int length = infile.tellg();
	cout<<"length :["<<length<<"]"<<endl;
	infile.seekg(0, ios_base::beg);
	char* buf =  new char[length + 1];
	infile.read(buf, length);
	string str(buf, length);
	delete[] buf;
	Trimmed(str);
	cout<<"read :["<<str<<"]"<<endl;
	string strHex;
	string strB64;
	if(argv[1][0] == '-' && argv[1][1] == 'c')
	{
		//strHex = EncodeHex(str);
		strB64 = EncodeBase64(str);
		//<<"Encode hex :["<<strHex<<"]"<<endl;
		cout<<"Encode base64 :["<<strB64<<"]"<<endl;
	}
	else if(argv[1][0] == '-' && argv[1][1] == 'd')
	{
		//strHex = DecodeHex(str);
		strB64 = DecodeBase64(str);
		//cout<<"Decode hex :["<<strHex<<"]"<<endl;
		cout<<"Decode base64 :["<<strB64<<"]"<<endl;
	}
	outfile.write(strB64.data(), strB64.size());
	cout<<"Ok"<<endl;
	
	return 0;
}
