#include "rector.h"

int rector::monitor()
{
	LOCK();
	timeval tv = {0};
	int timeout = 500; 
	gettimeofday(&tv,NULL) ;
	for (int i = 0; i < event_timer.size(); i++)
	{
		int tmp = dif_ms(event_timer[i]->when,tv);
		timeout = (timeout > tmp)?tmp:timeout;
	}
	timeout = (timeout < 0)?0:timeout;

	dmtpl_file_->handle(timeout);
	//long long tt1 = TimeFun::getCurrentTimeByMil();
	//long long ttt1, tttt1, ttt2, tttt2;
	for (std::map<int,Eventmask*>::iterator it = event_fire.begin(); it != event_fire.end(); it++)
	{
		std::map<int,fdFileEvent*>::iterator eve = events_reg.find(it->first);
		if (eve != events_reg.end())
		{
			if (*(it->second)&FD_READABLE&eve->second->mask)
			{
				//ttt1 = TimeFun::getCurrentTimeByMil();
				eve->second->read(it->first,eve->second->clientdata,eve->second->mask);
				//ttt2 = TimeFun::getCurrentTimeByMil();
			}
			if (*(it->second)&FD_WRITEABLE&eve->second->mask)
			{
				//tttt1 = TimeFun::getCurrentTimeByMil();
				eve->second->write(it->first,eve->second->clientdata,eve->second->mask);
				//tttt2 = TimeFun::getCurrentTimeByMil();
			}
		}
	}
	//long long tt2 = TimeFun::getCurrentTimeByMil();
	for (int i = 0; i < bad_events.size(); i++)
	{
		delete_file_event(bad_events[i]);
	}
	bad_events.clear();
	for (int i = 0; i < event_timer.size(); i++)
	{
		gettimeofday(&tv,NULL) ;
		if (cmptv(tv,event_timer[i]->when) >=0)
		{
			event_timer[i]->timeproc(event_timer[i]->id, event_timer[i]->clientdata);
			gettimeofday(&(event_timer[i]->when),NULL) ;
			inctv(&(event_timer[i]->when),event_timer[i]->interval);
		}
	}

	UNLOCK();
	return FD_OK;
}


rector::~rector(void)
{
	LOCK();
	for (std::map<int,fdFileEvent*>::iterator it = events_reg.begin(); it != events_reg.end(); it++)
	{
		delete it->second;
	}
	for (std::map<int,Eventmask*>::iterator it = event_fire.begin(); it != event_fire.end(); it++)
	{
		delete it->second;
	}

	for (int i = 0; i < event_timer.size(); i++)
	{
		delete event_timer[i];
	}
	events_reg.clear();
	event_fire.clear();
	event_timer.clear();
	if (dmtpl_file_) delete dmtpl_file_;
	//if(dmtpl_sig_) delete dmtpl_file_;
	
	UNLOCK();
}


int rector::unreg_timer_event(long long   id)
{
	LOCK();
	assert(id >0 && id < event_timer.size());
	if (id <0 || id >= event_timer.size())
	{
		UNLOCK();
		return FD_ERR;
	}

	delete event_timer[id];
	event_timer.erase(event_timer.begin()+id);
	UNLOCK();
	return FD_OK;
}


long long   rector::reg_timer_event(fdTimer* event)
{
	int ret = -1;
	assert(event);
	LOCK();
	// 	std::map<TIMERID,TIMER*>::iterator it  = event_timer.find(id);
	// 	if (it->second && it->second != event) delete it->second;
	event_timer.push_back(event);
	ret = event_timer.size();
	UNLOCK();
	return ret;
}


int rector::unreg_file_event(int id)
{
// 	LOCK();
// 	std::map<int,fdFileEvent*>::iterator it  = events_reg.find(id);
// 	dmtpl_file_->remove_handle(id);
// 	delete it->second;
// 	events_reg.erase(id);
// 	std::map<int,Eventmask*>::iterator it2 = event_fire.find(id);
// 	delete it2->second;
// 	event_fire.erase(id);
// 	UNLOCK();
// 	return FD_OK;
	LOCK();
	dmtpl_file_->remove_handle(id);
	bad_events.push_back(id);
	UNLOCK();
	return FD_OK;
}


int rector::delete_file_event(int id)
{
	LOCK();
	std::map<int,fdFileEvent*>::iterator it  = events_reg.find(id);
	dmtpl_file_->remove_handle(id);
	if (it != events_reg.end())
	{
		delete it->second;
		events_reg.erase(id);
	}
	else
	{
		int a = id;
	}
	std::map<int,Eventmask*>::iterator it2 = event_fire.find(id);
	if (it2 != event_fire.end())
	{
		delete it2->second;
		event_fire.erase(id);
	}
	else
	{
		int a = id;
	}
	UNLOCK();
	return FD_OK;
}

int rector::reg_file_event(int id,fdFileEvent* event)
{
	assert(event);
	LOCK();
	std::map<int,fdFileEvent*>::iterator it  = events_reg.find(id);
	if (it != events_reg.end() && it->second)
	{
		delete it->second;
	}
	events_reg[id] = event;
	std::map<int,Eventmask*>::iterator it2  = event_fire.find(id);
	if (it2 != event_fire.end() && it2->second)
	{
		delete it2->second;
	}
	Eventmask* pmask = new Eventmask();
	event_fire[id] = pmask;
	*pmask = 0;
	dmtpl_file_->add_handle(id,event->mask);
	UNLOCK();
	return FD_OK;
}

void rector::set_mask(int id, int mask)
{
	LOCK();
	std::map<int,fdFileEvent*>::iterator it  = events_reg.find(id);
	if (it != events_reg.end() && it->second)
	{
		it->second->mask = mask;
	}
	UNLOCK();
}

int rector::get_mask(int id)
{
	int ret = 0;
	LOCK();
	std::map<int,fdFileEvent*>::iterator it  = events_reg.find(id);
	if (it != events_reg.end() && it->second)
	{
		ret = it->second->mask;
	}
	UNLOCK();
	return ret;
}




