#ifndef _EVENT_POLL_H
#define _EVENT_POLL_H

#include "rector.h"


//typedef demultiplexer<int,fdFileEvent,fdTimer> BaseFileEventPoll;
class fdSelectPoll :
	public demultiplexer
{
	fd_set fd_set_r;
	fd_set _fd_set_r;
	fd_set fd_set_w;
	fd_set _fd_set_w;
	int maxfd;
public:
	fdSelectPoll(void);
	virtual ~fdSelectPoll(void);

	virtual int add_handle(int id,int mask);
	virtual int remove_handle(int id);
	virtual int handle(int timeout = 0);
};

#endif