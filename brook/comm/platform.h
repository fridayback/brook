#ifndef _PLATFORM_H
#define _PLATFORM_H
//ƽ̨�޹��Դ���

#ifdef WIN32


#include <ws2tcpip.h>
#include <WinSock2.h>
#include <windows.h>

#else
#error not include socket hander file
#endif
#include "synchronous.h "

#ifdef WIN32
#define snprintf _snprintf
#endif

#ifdef WIN32
//�߾��ȼ�ʱ
//LARGE_INTEGER

//���뼶��ʱ
int gettimeofday(struct timeval *tp, void *tzp);
#else
#endif

#ifdef WIN32
#define CLOSESOCKET closesocket 
#else
#define CLOSESOCKET close 
#endif

//tv1-tv2
long long dif_ms(timeval tv1,timeval tv2);
//tv1-tv2
int cmptv(timeval tv1,timeval tv2);
void inctv(timeval* tv,long long ms);
int initSocket();

#include "time_opt.h "

#include "crash_process.h"

unsigned int atomic_inc_int(unsigned* d);


//�߾���ʱ��
#ifdef WIN32
bool high_precision_timer_ini();
bool cur_time_(long long& bt);
long long dif_timer_tick(long long bt, long long et);
double  dif_timer_s(long long bt, long long et);
bool set_begin();
bool set_end();
long long get_dif();
double get_dif_s();
#endif


void fd_sleep(timeval tt);
#endif
