/*
* fdlog.cpp
*
*  Created on: 2014年8月14日
*      Author: ll
*/
#include "atomic_queue.h "
#include "fdthread.h "
#include "fdlog.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define BUFFSIZE 1024
#include "inifile.h"
#include <time.h>
int LOG_LEVEL = 2;

safe_queue<char*> log_queue;
static fd_thread* thhd = NULL;
unsigned __stdcall write_log(void * pParam);

unsigned __stdcall write_log(void * pParam)
{
	std::string path = app_path() + "dispatch.log";
#ifdef _WIN32
	FILE* fp = NULL;
	if (0 != fopen_s(&fp, path.c_str(), "a+"))
	{
		if (fp) fclose(fp);
		fp = NULL;
	}
#else
	FILE* fp = fopen(path.c_str(), "a+");
#endif
	if (fp)
	{
		char* p = NULL;
		while (log_queue.pop(&p))
		{
			fwrite(p, strlen(p), 1, fp);
			delete[]  p;
			//printf("delete %p\n",p);
		}
		fclose(fp);
	}
	else
		printf("open %s fail when wirte log\n", path.c_str());

#if WIN32
	Sleep(500);
#else
#endif
	return 0;
}


void Log(int lv, const char* format, ...)
{
#if 1
	//if(lv < LOG_LEVEL) return;
	bool bwf = false;
	bool bwc = false;
	if (lv < 100)
	{
		bwf = true;
		bwc = true;
	}
	else if (lv >= 200)
	{
		bwf = true;
		lv -= 200;
	}
	else
	{
		bwc = true;
		lv -= 100;
	}

	if (lv < LOG_LEVEL) return;
	int rate = 1;
	char* buf = new char[BUFFSIZE*rate + 1];
	//memset(buf,0,BUFFSIZE*rate+1);
	//printf("new %p\n",buf);
	va_list args;

	va_start(args, format);
	//vsprintf(p, format, args);
#ifdef _WIN32
	while (_vsnprintf_s(buf, BUFFSIZE*rate, _TRUNCATE, format, args) < 0)
#else

#endif
	{
		delete[] buf;
		//printf("delete %p\n",buf);
		rate++;
		buf = new char[BUFFSIZE*rate + 1];
		//memset(buf,0,BUFFSIZE*rate+1);
		//printf("new %p\n",buf);
	}

	va_end(args);
	//此处两个赋值语句不能颠倒顺序
	int len = strlen(buf);
	if (len >= BUFFSIZE*rate)
	{
		char* tmp = new char[len + 2];
		memcpy(tmp, buf, len);
		delete[] buf;
		buf = tmp;
	}
	buf[len + 1] = 0;
	buf[len] = '\n';
	//std::string path = app_path() + "\\server.log";
	time_t timep;
	tm pt;
	time(&timep);
#ifdef _WIN32
	if (0 != localtime_s(&pt, &timep))
	{

	}
#else
	localtime_r(&timep, &pt);
#endif
	char* timestr = new char[50];
	//printf("new %p\n",timestr);
	memset(timestr, 0, 50);
	std::string loglv = "";
	switch (lv)
	{
	case 0:
	case 100:
		loglv = "[TRACE] :";
		break;
	case 1:
	case 101:
		loglv = "[DEBUG] :";
		break;
	case 2:
	case 102:
		loglv = "[INFO]  :";
		break;
	case 3:
	case 103:
		loglv = "[WARNING]  :";
		break;
	case 4:
	case 104:
		loglv = "[ERROR] :";
		break;
	default:;
	}
	sprintf_s(timestr, sizeof(char) * 50, "[%04d-%02d-%02d %02d:%02d:%02d] %s", pt.tm_year + 1900, pt.tm_mon + 1, pt.tm_mday, pt.tm_hour, pt.tm_min, pt.tm_sec, loglv.c_str());
	if (bwc)printf_s("%s%s", timestr, buf);
	if (bwf)
	{
		log_queue.push(timestr);
		log_queue.push(buf);
	}
	else
	{
		delete[] timestr;
		//printf("delete %p\n",timestr);
		delete[] buf;
		//printf("delete %p\n",buf);
	}

#endif
}
static bool bInit = false;
void log_init()
{
	if (bInit)
	{
		return;
	}
	bInit = true;
	thhd = new fd_thread();
	thhd->run(write_log, NULL);
}

void log_uninit()
{
	if (thhd)
	{
		thhd->terminate();
		delete thhd;
	}
	char* p = NULL;
	while (log_queue.pop(&p))
	{
		delete[] p;
	}
	bInit = false;
}

void log_hex(int lv, char* buf, int len, char* format, ...)
{
	//if(lv < LOG_LEVEL) return;
	bool bwf = false;
	bool bwc = false;
	if (lv < 100)
	{
		bwf = true;
		bwc = true;
	}
	else if (lv >= 200)
	{
		bwf = true;
		lv -= 200;
	}
	else
	{
		bwc = true;
		lv -= 100;
	}

	if (lv < LOG_LEVEL) return;

	int rate = 1;
	char* msgappend = new char[BUFFSIZE*rate + 1];
	//memset(buf,0,BUFFSIZE*rate+1);
	//printf("new %p\n",buf);
	va_list args;

	va_start(args, format);
	//vsprintf(p, format, args);
#ifdef _WIN32
	while (_vsnprintf_s(msgappend, BUFFSIZE*rate, _TRUNCATE, format, args) < 0)
#else
	while (_vsnprintf(msgappend, BUFFSIZE*rate, format, args) < 0)
#endif
	{
		delete[] msgappend;
		//printf("delete %p\n",buf);
		rate++;
		msgappend = new char[BUFFSIZE*rate + 1];
		//memset(buf,0,BUFFSIZE*rate+1);
		//printf("new %p\n",buf);
	}

	va_end(args);

	char *hexstr = new char[3 * len + 2];
	for (int i = 0; i < len; i++)
	{
		sprintf_s(&hexstr[3 * i], 3 * len + 2, "%02X ", (unsigned char)buf[i]);
	}
	hexstr[3 * len] = '\n';
	hexstr[3 * len + 1] = 0;

	time_t timep;
	tm pt;
	time(&timep);
#ifdef _WIN32
	localtime_s(&pt, &timep);
#else
	localtime_r(&timep, &pt);
#endif
	char* timestr = new char[50];
	memset(timestr, 0, 50);
	std::string loglv = "";
	switch (lv)
	{
	case 0:
	case 100:
		loglv = "[TRACE] :";
		break;
	case 1:
	case 101:
		loglv = "[DEBUG] :";
		break;
	case 2:
	case 102:
		loglv = "[INFO]  :";
		break;
	case 3:
	case 103:
		loglv = "[WARNING]  :";
		break;
	case 4:
	case 104:
		loglv = "[ERROR] :";
		break;
	default:;
	}
#ifdef _WIN32
	sprintf_s(timestr, 50, "[%04d-%02d-%02d %02d:%02d:%02d] %s", pt.tm_year + 1900, pt.tm_mon + 1, pt.tm_mday, pt.tm_hour, pt.tm_min, pt.tm_sec, loglv.c_str());
#else
	sprintf(timestr, "[%04d-%02d-%02d %02d:%02d:%02d] %s", pt.tm_year + 1900, pt.tm_mon + 1, pt.tm_mday, pt.tm_hour, pt.tm_min, pt.tm_sec, loglv.c_str());
#endif
	if (bwc) printf("%s%s%s", timestr, msgappend, hexstr);
	if (bwf)
	{
		log_queue.push(timestr);
		log_queue.push(msgappend);
		log_queue.push(hexstr);
	}
	else
	{
		delete[] msgappend;
		delete[] timestr;
		//printf("delete %p\n",timestr);
		delete[] hexstr;
		//printf("delete %p\n",buf);
	}
}

