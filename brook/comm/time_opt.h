#ifndef _TIME_OPT_H
#define _TIME_OPT_H
#include<string>



namespace TimeFun
{
	long long getCurrentTimeByMil();
	std::string GetLocalTime();
	std::string MilSecToStr(long long milsec,std::string format = "YYYY-MM-DD HH:mm:SS");
	long long string2milsec(std::string time_str,std::string format = "YYYY-MM-DD HH:mm:SS");
	struct tm localtime(time_t* t);
}
#endif
