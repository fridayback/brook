#ifndef _EVENTS_H
#define _EVENTS_H

#include "platform.h"

typedef enum CALL_BACK_T
{
	cRead = 0,
	cWrite,
	cError,


	cEnd
};

typedef int fdFileProc( int fd, void *clientData, int mask);
typedef int fdTimeProc(long long  id, void *clientData);
// typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);
// typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);

typedef struct fdFileEvent
{
	fdFileProc* read;
	fdFileProc* write;
	int mask;
	void* clientdata;
}fdFileEvent;

typedef struct fdTimer
{
	long long id;
	fdTimeProc* timeproc;
	timeval when;
	long long interval;//∫¡√Î
	void* clientdata;
}fdTimer;

typedef struct benodEvent
{
	long long id;
	int mask;
	fdTimeProc* read;
	fdTimeProc* write;
	void* clientdata;
}benodEvent;

#endif

