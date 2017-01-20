#ifndef _FD_THREAD_H
#define _FD_THREAD_H


#ifdef _WIN32
#include <windows.h>
#define STDCALL __stdcall
#define THREAD HANDLE
#define THREADFUNRETURN unsigned int
typedef unsigned __stdcall THREAD_FN(void * pParam);
#else 

// Ä¿Ç°Ä¬ÈÏºìÆì64
#include <pthread.h>
#define STDCALL
#define THREAD void* 
#define THREADFUNRETURN void*
typedef void* THREAD_FN(void * pParam);
#endif  

class fd_thread
{
	unsigned int id;
	THREAD handle;
	THREAD_FN* fn;
	bool bexit; 
	typedef struct thread_param
	{
		fd_thread* pthis;
		void* other;
	}thread_param;
	thread_param th_par;
#ifdef WIN32
	static unsigned __stdcall ThreadFn(void* pParam);
#else
	static void* ThreadFn(void* pParam);
#endif
public:
	fd_thread(void);
	THREAD get_handle();
	int wait_exit();
	unsigned int getthreadid();
	bool run(THREAD_FN* f,void* pParam);
	bool terminate();
	virtual ~fd_thread(void);
public:
	static bool EXIT;
};

#endif