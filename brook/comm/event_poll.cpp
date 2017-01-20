#include "event_poll.h"
#include "fdlog.h"

fdSelectPoll::fdSelectPoll(void)
{
	FD_ZERO(&fd_set_r);//相对固定
	FD_ZERO(&_fd_set_r);//每次执行handle拷贝上面
	FD_ZERO(&fd_set_w);
	FD_ZERO(&_fd_set_w);
	maxfd = -1;
}


fdSelectPoll::~fdSelectPoll(void)
{
}

int fdSelectPoll::add_handle(int id,int mask)
{
	//rector_->event_fire[id] = 1;
	LOG_INFO("add_handle:fd = %d, maks = %d",id,mask);
	if (mask & FD_READABLE) FD_SET(id,&fd_set_r);
	if (mask & FD_WRITEABLE) FD_SET(id,&fd_set_w);
	maxfd = id > maxfd?id:maxfd;
	return 0;
}

int fdSelectPoll::remove_handle(int id)
{
	FD_CLR(id,&fd_set_r);
	FD_CLR(id,&fd_set_w);
	return 0;
}
#include <stdlib.h>
int fdSelectPoll::handle(int timeout)
{
	timeval tv;
	tv.tv_sec = timeout/1000;
	tv.tv_usec = (timeout%1000)*1000;
	FD_ZERO(&_fd_set_r);
	FD_ZERO(&_fd_set_w);
	memcpy(&_fd_set_r,&fd_set_r,sizeof(fd_set));
	memcpy(&_fd_set_w,&fd_set_w,sizeof(fd_set));

	for (std::map<int, fdFileEvent*>::iterator it = rector_->events_reg.begin(); it != rector_->events_reg.end(); it++)
	{
		int fd = *(int*)it->second->clientdata;
		if (0 == (it->second->mask & FD_WRITEABLE)) FD_CLR(fd, &_fd_set_w);
	}
	
	int retval = select(maxfd+1,&_fd_set_r,&_fd_set_w,NULL,&tv);

	for (std::map<int,fdFileEvent*>::iterator it = rector_->events_reg.begin(); it != rector_->events_reg.end(); it++)
	{
		if (it->second->mask == FD_NONE) continue;
		if (it->second->mask & FD_READABLE && FD_ISSET(it->first,&_fd_set_r))  
			*(rector_->event_fire[it->first])  |= FD_READABLE;
		else
			*(rector_->event_fire[it->first])  &= ~FD_READABLE;
		if (it->second->mask & FD_WRITEABLE && FD_ISSET(it->first,&_fd_set_w)) 
			*(rector_->event_fire[it->first])  |= FD_WRITEABLE;
		else
			*(rector_->event_fire[it->first])  &= ~FD_WRITEABLE;
	}

	return 0;
}
