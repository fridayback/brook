#include "platform.h"
#include <time.h>

#ifdef WIN32
int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year     = wtm.wYear - 1900;
	tm.tm_mon     = wtm.wMonth - 1;
	tm.tm_mday     = wtm.wDay;
	tm.tm_hour     = wtm.wHour;
	tm.tm_min     = wtm.wMinute;
	tm.tm_sec     = wtm.wSecond;
	tm. tm_isdst    = -1;
	clock = mktime(&tm);
	if(clock < 0) throw "monitor():gettimeofday()";
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return (0);
}
#endif

long long dif_ms(timeval tv1,timeval tv2)
{
	return (tv1.tv_usec+1000000 -tv2.tv_usec)/1000LL + (tv1.tv_sec -tv2.tv_sec -1)*1000LL;
}



int cmptv(timeval tv1,timeval tv2)
{
	int ret = tv1.tv_sec - tv2.tv_sec;
	if (ret == 0) ret = tv1.tv_usec - tv2.tv_usec;
	if (ret == 0) return 0;
	if(ret < 0) return -1;
	return 1;
}

void inctv(timeval* tv,long long ms)
{
	tv->tv_usec += (ms%1000)*1000;
	tv->tv_sec += ms/1000L + tv->tv_usec/1000000L;
	tv->tv_usec %= 1000000;
}

int initSocket()
{
#ifdef WIN32
	//the version bit of Winsock      
	int version_a = 1;//low bit      
	int version_b = 1;//high bit       
	//makeword      
	WORD versionRequest = MAKEWORD(version_a,version_b);      
	WSAData wsaData;      
	int error;      
	error = WSAStartup(versionRequest, &wsaData);        
	if(error != 0)
	{          
		printf("ERROR!");          
		return -1;      
	}      
	//check whether the version is 1.1, if not print the error and cleanup wsa?      
	if (LOBYTE(wsaData.wVersion)  != 1 || HIBYTE(wsaData.wVersion) != 1)      
	{          
		printf("WRONG WINSOCK VERSION!");          
		WSACleanup();          
		return -1;      
	}
	return 1;
#else
	return 1;
#endif
}

#ifdef WIN32
unsigned int atomic_inc_int(unsigned* d)
{
	return InterlockedIncrement(d);
}


#else
#error atomic_inc_int not implemte;
#endif


#ifdef WIN32
static LARGE_INTEGER lv = {0};
static LARGE_INTEGER bt,et;
static double msPerTick = 0.;
//////////////////////////////////////////////////////////////////////////
bool high_precision_timer_ini()
{
	if(!QueryPerformanceFrequency( &lv )) return false;
	// 转换为每个Tick多少秒
	msPerTick = 1000000.0 / lv.QuadPart;
	bt.QuadPart = 0;
	et.QuadPart = 0;
	return true;
}

bool cur_time_(long long& bt)
{
	LARGE_INTEGER l = {0};
	if(!QueryPerformanceCounter(&l)) return false;
	bt = l.QuadPart;

	return true;
}


long long  dif_timer_tick(long long bt, long long et)
{
	return  et - bt;
}

double  dif_timer_s(long long bt, long long et)
{
	double  l;
	l = (et - bt)*msPerTick;
	return l;
}

bool set_begin()
{
	return QueryPerformanceCounter(&bt);
}

bool set_end()
{
	return QueryPerformanceCounter(&et);
}

long long get_dif()
{
	return dif_timer_tick(bt.QuadPart,et.QuadPart);
}

double get_dif_s()
{
	return dif_timer_s(bt.QuadPart,et.QuadPart);
}

#else

#endif


void fd_sleep(timeval tt)
{
 #ifdef WIN32
 	Sleep(tt.tv_sec*1000+tt.tv_usec/1000);
 #else
// 	timeval t;
// 	t = tt;
	select(0,NULL,NULL,NULL,&tt);
#endif
}