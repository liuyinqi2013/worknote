#ifndef __H__H__LINUXBASE
#define __H__H__LINUXBASE

#include <string>
using namespace std;

int GetOsName(string& osName);

int GetOSVersion(string& osVersion);

int GetOSUID(string& osUID);

int GetOSInstallTime(string& installTime);

int GetPowerOnTime(string& onTime);

int GetPowerOffTime(string& offTime);


#endif