#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "LinuxBase.h"

#ifndef WIN32
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <errno.h>
#include <utmp.h>

int GetOsName(string& osName)
{
	osName = "Linux";
	return 0;
}

int GetOSVersion(string& osVersion)
{
	FILE* fp = fopen("/etc/issue", "r");
	if(fp == NULL)
	{
		return -1;
	}

	char buf[256] = {0};

	if(fgets(buf, sizeof(buf), fp))
	{
		char *p = strrchr(buf, '\n');
		if(p) {
			*p = '\0';
			osVersion =  buf;
		}
	}
	fclose(fp);

	return 0;
}

int GetOSUID(string& osUID)
{
	FILE* fp = fopen("/sys/class/dmi/id/product_uuid", "r");
	if(fp == NULL)
	{
		return -1;
	}

	char buf[256] = {0};
	if(fgets(buf, sizeof(buf), fp))
	{
		char *p = strrchr(buf, '\n');
		if(p) {
			*p = '\0';
			osUID = buf;
		}
	}
	fclose(fp);

	return 0;
}

int GetOSInstallTime(string& installTime)
{
	struct stat stat_buf;
	struct tm *tmp;
	if(stat("/lost+found", &stat_buf) < 0)
	{
		return -1;
	}

	tmp = localtime(&stat_buf.st_mtime);

	if(tmp == NULL)
	{
		perror("lost+found localtime get faild");
		return -1;
	}

	char buf[256] = {0};
	sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d", 
		tmp->tm_year+1900,
		tmp->tm_mon+1,
		tmp->tm_mday,
		tmp->tm_hour,
		tmp->tm_min,
		tmp->tm_sec);

	installTime = buf;

	return 0;
}

int GetPowerOnTime(string& onTime)
{
	struct sysinfo info;
	time_t cur_time = 0;
	time_t boot_time = 0;
	struct tm *tmp = NULL;
	if(sysinfo(&info)) 
	{
		fprintf(stderr, "Failed to get sysinfo, errno:%u, reason:%s\n",
			errno, strerror(errno));
		return -1;
	}

	time(&cur_time);

	if (cur_time > info.uptime) 
	{
		boot_time = cur_time - info.uptime;
	}
	else 
	{
		boot_time = info.uptime - cur_time;
	}

	tmp = localtime(&boot_time);

	char buf[256] = {0};
	sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d", 
		tmp->tm_year+1900,
		tmp->tm_mon+1,
		tmp->tm_mday,
		tmp->tm_hour,
		tmp->tm_min,
		tmp->tm_sec);
	onTime = buf;

	return 0;
}

int GetPowerOffTime(string& offTime)
{
	struct utmp *u ;
	struct tm *tmp;

	utmpname("/var/log/wtmp");
	while((u = getutent()))
	{
		if(strncmp(u->ut_user,"shutdown",8) == 0)
		{
			time_t timep = (time_t)u->ut_tv.tv_sec;
			tmp = localtime(&timep);
		}
	}
	endutent();

	char buf[256] = {0};
	sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d", 
		tmp->tm_year+1900,
		tmp->tm_mon+1,
		tmp->tm_mday,
		tmp->tm_hour,
		tmp->tm_min,
		tmp->tm_sec);
	offTime = buf;
}

#endif