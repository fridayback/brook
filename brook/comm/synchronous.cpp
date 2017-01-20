/*
* synchronous.cpp
*
*  Created on: 2015-07-11
*      Author: liulin
*/
#include "synchronous.h"
#include <assert.h>
#ifdef _WIN32


criticalsec_::criticalsec_()
{
	InitializeCriticalSection(&m_Section);
}

criticalsec_::~criticalsec_()
{
	DeleteCriticalSection(&m_Section);
}

int criticalsec_::Lock()
{
	EnterCriticalSection(&m_Section);

	return 0;
}

int criticalsec_::UnLock()
{
	LeaveCriticalSection(&m_Section);
	return 0;

}
#else
int criticalsec_::UnLock()
{
	return pthread_mutex_unlock(&m_Section);//����
}



int criticalsec_::Lock()
{
	return pthread_mutex_lock(&m_Section);
}



criticalsec_::criticalsec_()
{
	pthread_mutexattr_init(&m_mattr);
	pthread_mutexattr_setpshared(&m_mattr, PTHREAD_PROCESS_PRIVATE);//进程内可用
	pthread_mutexattr_settype(&m_mattr, PTHREAD_MUTEX_RECURSIVE);//同一线程可重复锁,同时需要多次打开
	pthread_mutex_init(&m_Section, &m_mattr);
}



criticalsec_::~criticalsec_()
{
	pthread_mutex_destroy(&m_Section);
	pthread_mutexattr_destroy(&m_mattr);
}
#endif




signal_::signal_(void)
{
#ifdef _WIN32
	//hMutex = CreateMutex(NULL,FALSE,NULL);
	hMutex = CreateSemaphore(NULL, 1, 1, NULL);
#else
	sem_init(&sem_id, 0, 1);
	p_semid = NULL;
#endif
}


signal_::signal_(const char* name)
{
#ifdef _WIN32
	//hMutex = CreateMutex(NULL,FALSE,name);
	hMutex = CreateSemaphore(NULL, 1, 1, name);
#else
	p_semid = sem_open(name, O_CREAT | O_EXCL, FILE_MODE, 0);
	sem_id = 0;
#endif
}

signal_::~signal_(void)
{
#ifdef _WIN32
	CloseHandle(hMutex);
#else
	if (semid) sem_destroy(semid);
	if (p_semid) sem_unlink(p_semid);
#endif
}

bool signal_::take(int milsec)
{
#ifdef _WIN32
	int ret = WaitForSingleObject(hMutex, milsec);
	assert(ret != -1);
	return (WAIT_OBJECT_0 == ret);
#else
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec = 1000000(milsec % 1000);
	ts.tv_sec += milsec / 1000 + ts.tv_nsec / 1000000;
	ts.tv_nsec %= 1000000;
	sem_t* tmp = NULL:
	if (semid)
	{
		tmp = &semid;
	}
	if (p_semid)
	{
		tmp = p_semid;
	}
	int s = 0;
	while ((s = sem_timedwait(tmp, &ts)) == -1 && errno == EINTR) continue;
	assert(s == -1 && errno != ETIMEDOUT);
	return (s != -1);
#endif
}

void signal_::release()
{
#ifdef _WIN32
	//ReleaseMutex(hMutex);
	long pre = 0;
	ReleaseSemaphore(hMutex, 1, &pre);
#else
	sem_t* tmp = NULL:
	if (semid)
	{
		tmp = &semid;
	}
	if (p_semid)
	{
		tmp = p_semid;
	}
	sem_post(tmp);
#endif
}

#include "time_opt.h"
bool fdlock_::lock(int timeout /*= -1*/)
{
#ifdef _WIN32
	unsigned long thrid = (unsigned long)GetCurrentThread();
	unsigned long tmp = 0;
	long long t = TimeFun::getCurrentTimeByMil();
	while (tmp = InterlockedCompareExchange(&fg, thrid, 0))
	{
		//如果线程不存在了,自动解除锁定,然后在用新线程加锁
		if (0 == WaitForSingleObject((HANDLE)tmp, 0))
		{
			//解锁并重新加锁
			InterlockedCompareExchange(&fg, thrid, fg);
			//判断重新加锁是否成功
			if (thrid == InterlockedCompareExchange(&fg, 0, 0))
			{
				return true;
			}
		}

		if (timeout != -1 && TimeFun::getCurrentTimeByMil() - t >= timeout) return false;
	}

	return true;
#else
#error fdlock_ not implement
#endif
}

bool fdlock_::unlock()
{
#ifdef _WIN32
	unsigned long thrid = (unsigned long)GetCurrentThread();
	return (thrid == InterlockedCompareExchange(&fg, 0, thrid));
#else
#error fdlock_ not implement
#endif
}