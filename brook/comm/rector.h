#ifndef _RECTOR_H
#define _RECTOR_H

#include <map>
#include <vector>
#include <assert.h>
#include "events.h"
#ifndef THREAD_SALF
//#include "common/synchronous.h"
#endif

#ifdef THREAD_SALF
#define LOCK() lock.Lock()
#define UNLOCK() lock.UnLock()
#define DECLEAR_LOCK criticalsec_ lock;
#else
#define LOCK() 
#define UNLOCK() 
#define DECLEAR_LOCK 
#endif

#define FD_OK 0
#define FD_ERR -1
#define FD_NONE            0x00000000
#define FD_READABLE   0x00000001
#define FD_WRITEABLE 0x00000002


class rector;
typedef rector BeNodeSrv;
/*******************
//  ��·���÷�����
*******************/
class demultiplexer
{
	friend class rector;
	//void* handle_data;
	DECLEAR_LOCK
protected:
	rector * rector_;
public:
	demultiplexer(void){};
	virtual ~demultiplexer(void)=0{};
	virtual int add_handle(int id,int mask)=0;
	virtual int remove_handle(int id)=0;
	virtual int handle(int timeout = 0)=0;
};

typedef int Eventmask;
 
/*******************
//         ��Ӧ��
*******************/
class rector
{
	DECLEAR_LOCK
protected:
		virtual int delete_file_event(int id);
public:
	std::map<int,fdFileEvent*> events_reg;//��������ע���¼�
	std::map<int,Eventmask*> event_fire; //�������¼��б�
	std::vector<fdTimer*> event_timer;//��ʱ�¼�
	demultiplexer* dmtpl_file_;
	//demultiplexer* dmtpl_sig_;
	std::vector<int> bad_events;//�������л����ļ��¼�
	//std::vector<int> bad_timer;//�������л��Ķ�ʱ���¼�
public:
	rector(void){};
	rector(demultiplexer* d):dmtpl_file_(d)/*,dmtpl_sig_(0)*/
	{
		dmtpl_file_->rector_ = this;
		//dmtpl_sig_->rector_ = this;
	}

	virtual ~rector(void);

	virtual int reg_file_event(int id,fdFileEvent* event);
	virtual void set_mask(int id, int mask);
	virtual int get_mask(int id);
	virtual int unreg_file_event(int id);
	virtual long long   reg_timer_event(fdTimer* event);
	virtual int unreg_timer_event(long long   id);
	virtual int monitor();
};

#endif