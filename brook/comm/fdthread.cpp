
#include "fdthread.h"



#ifdef WIN32
#include <process.h>
bool fd_thread::EXIT = false;
#define STDCALL __stdcall
#define THREAD HANDLE
#define THREADFUNRETURN unsigned int
typedef unsigned __stdcall THREAD_FN(void * pParam);
#else 

// Ä¿Ç°Ä¬ÈÏºìÆì64
#include <pthread.h>
bool fd_thread::EXIT = false;
#define STDCALL
#define THREAD void* 
#define THREADFUNRETURN void*

#endif  

#ifdef WIN32
unsigned __stdcall fd_thread::ThreadFn(void* pParam)
#else
void* fd_thread::ThreadFn(void* pParam)
#endif
{
	thread_param* pobj = reinterpret_cast<thread_param*>(pParam);
	while(!pobj->pthis->bexit)
	{
		pobj->pthis->fn(pobj->other);
	}

	return 0;
}

fd_thread::fd_thread(void):id(0),handle(0),bexit(false)
{
}

THREAD fd_thread::get_handle()
{
	return handle;
}

int fd_thread::wait_exit()
{
#ifdef WIN32
	return WaitForSingleObject(handle,INFINITE);//INFINITE
#else
	return ::pthread_join((pthread_t)handle, 0);
#endif
}

unsigned int fd_thread::getthreadid()
{
	return id;
}

bool fd_thread::run(THREAD_FN* f,void* pParam)
{
	th_par.pthis = this;
	th_par.other = pParam;
	fn = f;
#ifdef WIN32
	handle =  reinterpret_cast<HANDLE>(_beginthreadex(NULL,0,ThreadFn,&th_par,0,&id));
	return (0 != handle);
#else
	return 0 == ::pthread_create((pthread_t*)&handle, NULL, (void*(*)(void*))ThreadFn, &th_par);
	 return (id == 0);
#endif
}

bool fd_thread::terminate()
{
	bexit = true;
	EXIT = true;
#ifdef WIN32
	return (WAIT_OBJECT_0 == wait_exit());
#else
	return (0 == wait_exit());
#endif
}

fd_thread::~fd_thread(void)
{
}
