#ifndef _ATOMIC_QUEUE_H  
#define _ATOMIC_QUEUE_H  

#ifdef _WIN32
#include <windows.h>  
#include "synchronous.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "synchronous.h"

//#define NULL ((void*)0)
typedef void *PVOID;
#define CRITICAL_LOCK

#endif
//如果用原子操作注释该宏，否则打开
//#define CRITICAL_LOCK

template<class QElmType>  
struct qnode  
{  
	struct qnode *prev, *next;  
	QElmType data;  
};  

template<class QElmType>  
class safe_queue  
{  
public:  
	safe_queue()   
	{  
		m_front = new qnode<QElmType>;  
		m_front->next = 0;  
		m_rear = m_front; 
	}  
	~safe_queue() 
	{  
		while (m_front)  
		{  
			qnode<QElmType>* tmp = m_front;
			m_front = m_front->next;  
			delete tmp;
		}  
	}  
	//将新的节点放入队列尾部  
	bool push(QElmType e)  
	{  
		//新建一个节点  
		struct qnode<QElmType> *p = new qnode<QElmType>;  
		if (!p)  
			return false;  
		p->next = 0;  
#ifdef CRITICAL_LOCK
		lock.Lock();
		p->prev = m_rear;  
		m_rear->next = p;  
        m_rear->data = e;
		m_rear = p;
		_size++;
		lock.UnLock();
#else
		while (1)  
		{  
			struct qnode<QElmType> *rear = m_rear;  
			p->prev = rear; 
			p->next = NULL;
			//p->data = NULL;
			//rear->data = e;
			//rear->next = p;
			
			//如果队列没有改变将p赋值给m_rear，否则继续循环  
			if (InterlockedCompareExchangePointer((volatile PVOID *)&m_rear,   
				p, rear) == rear)  
			{  
				rear->data = e;
 				rear->next = p;
				InterlockedIncrement(&(this->_size));
				break;  
			}  
		}  
#endif
		return true;  
	}  
	//从队列头部取出一个节点  
	bool pop(QElmType *e)  
	{  
#ifdef CRITICAL_LOCK
		
		lock.Lock();
		struct qnode<QElmType> *front = m_front;  
		if (front == m_rear)  
		{
			lock.UnLock();
			return false; 
		}
		*e = front->data;
		m_front = m_front->next;
		delete front;
		_size--;
		lock.UnLock();
#else
		while (1)  
		{  
			struct qnode<QElmType> *front = m_front;  
			if (front == m_rear)  
				return false;  
			if (front->next ==NULL)
			{
				continue;
			}
			//如果队列位置没有变将m_front指向下一个位置,否则继续循环  
			if(InterlockedCompareExchangePointer((volatile PVOID *)&m_front,  
				m_front->next, front) == front)  
			{  
				*e = front->data;  
				delete front;  
				InterlockedDecrement(&(this->_size));
				return true;  
			}  
		}  
#endif
		return true;  
	}  

	unsigned long size()
	{
#ifdef CRITICAL_LOCK
		return _size;
#else
		unsigned long tmp = -1;
		return InterlockedCompareExchange(&(this->_size),-1,tmp);
#endif
	}
private:  
	struct qnode<QElmType> *m_front, *m_rear;  
	unsigned long _size;
#ifdef CRITICAL_LOCK
	criticalsec_ lock;
#endif
};  

#endif