/*
* synchronous.h
*
*  Created on: 2015-07-11
*      Author: liulin
*/

#ifndef CRITICALSECTION_H_
#define CRITICALSECTION_H_

#include <string>
#ifdef _WIN32
#include <Windows.h>
#define CRITICALSECTION CRITICAL_SECTION
#define SEMAPHORE HANDLE
#else // Ä¿Ç°Ä¬ÈÏºìÆì64
#include <pthread.h>
#include <semaphore.h>
#define CRITICALSECTION  pthread_mutex_t
#endif  //_WIN32


class criticalsec_
{
private:
	CRITICALSECTION m_Section;
#ifndef _WIN32
	pthread_mutexattr_t m_mattr;
#endif
public:
	criticalsec_();
	~criticalsec_();
	int Lock();
	int UnLock();

};

class signal_
{
#ifdef _WIN32
	HANDLE hMutex;
#else
	sem_t semid;
	sem_t* p_semid;
#endif

public:
	signal_(void);
	signal_(const char* name);
	virtual ~signal_(void);
	bool take(int milsec = 0);
	void release();
};


class fdlock_
{
	volatile unsigned long fg;
public:
	fdlock_()
	{
		fg = 0;
	}
	~fdlock_() {}
	bool lock(int timeout = -1);


	bool unlock();

};


#endif /* CRITICALSECTION_H_ */
