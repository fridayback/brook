#include "time_opt.h"
#include "platform.h"
#include <time.h>
namespace TimeFun
{
		typedef struct FORMAT_CONVER
	{
		std::string tm_fmt;
		std::string io_fmt;
	}FORMAT_CONVER;
	const FORMAT_CONVER FORMAT_INFACTOR[] = {
		{"YYYY-MM-DD HH:mm:SS","%04d-%02d-%02d %02d:%02d:%02d"},
		{"YYYY-MM-DD HH:mm:SS.sss","%04d-%02d-%02d %02d:%02d:%02d.%03d"}
	};
	long long getCurrentTimeByMil()
	{
		struct timeval tv;
		gettimeofday(&tv,NULL);
		return tv.tv_sec * 1000LL + tv.tv_usec / 1000LL;
	}

	std::string GetLocalTime()
	{
		time_t timep;
		struct tm p;
		time(&timep);
#ifdef _WIN32
		localtime_s(&p,&timep);
#else
		localtime_r(&timep,&p);
#endif
		char timestr[30] = "";
#ifdef _WIN32
		sprintf_s(timestr,"%04d-%02d-%02d %02d:%02d:%02d",p.tm_year+1900
			,p.tm_mon+1,p.tm_mday,p.tm_hour,p.tm_min,p.tm_sec);
#else
		sprintf(timestr, "%04d-%02d-%02d %02d:%02d:%02d", p.tm_year + 1900
			, p.tm_mon + 1, p.tm_mday, p.tm_hour, p.tm_min, p.tm_sec);
#endif
		return timestr;
	}

	std::string MilSecToStr( long long milsec ,std::string format)
	{
		std::string fmt = "";
		for (int i = 0; i < sizeof(FORMAT_INFACTOR)/sizeof(FORMAT_INFACTOR[0]); i++)
		{
			if (FORMAT_INFACTOR[i].tm_fmt == format)
			{
				fmt = FORMAT_INFACTOR[i].io_fmt;
			}
		}
		if (fmt == std::string("")) return fmt;

		time_t t;
		struct tm p;
		t = milsec/1000LL;
#ifdef _WIN32
		localtime_s(&p, &t);
#else
		localtime_r(&t, &p);
#endif
		char timestr[30] = "";
#ifdef _WIN32
		sprintf_s(timestr,fmt.c_str(),p.tm_year+1900,p.tm_mon+1,p.tm_mday
			,p.tm_hour,p.tm_min,p.tm_sec);
#else
		sprintf(timestr, fmt.c_str(), p.tm_year + 1900, p.tm_mon + 1, p.tm_mday
			, p.tm_hour, p.tm_min, p.tm_sec);
#endif
		return timestr;
	}

	
	long long string2milsec(std::string time_str,std::string format)
	{
		std::string fmt = "";
		for (int i = 0; i < sizeof(FORMAT_INFACTOR)/sizeof(FORMAT_INFACTOR[0]); i++)
		{
			if (FORMAT_INFACTOR[i].tm_fmt == format)
			{
				fmt = FORMAT_INFACTOR[i].io_fmt;
			}
		}
		if (fmt == std::string("")) return -1;
		struct tm tt;
		int cnt = 0;
		int milsec = 0;
#ifdef _WIN32
		if(6 > (cnt =sscanf_s(time_str.c_str(),fmt.c_str(),&tt.tm_year,&tt.tm_mon,&tt.tm_mday,&tt.tm_hour,&tt.tm_min,&tt.tm_sec,&milsec)))
#else
		if (6 > (cnt = sscanf(time_str.c_str(), fmt.c_str(), &tt.tm_year, &tt.tm_mon, &tt.tm_mday, &tt.tm_hour, &tt.tm_min, &tt.tm_sec, &milsec)))
#endif
		{
			return -1;
		}
		if (cnt < 7) milsec = 0;
		tt.tm_year -= 1900;
		tt.tm_mon -= 1;

		long long ret = mktime(&tt);
		return ret*1000+milsec;
	}

	struct tm localtime(time_t* t)
	{

		struct tm tt = { 0 };
#ifdef WIN32
		localtime_s(&tt, t);
#else
		localtime_r(t, &tt);
#endif
		return tt;
	}

}

