#ifndef _CQ_H
#define _CQ_H
#include "fdlog.h"
//循环队列
#define DEF_QUEUE_LEN (131200)
template<class T>
class cq
{
	int len;
	int first;
	//int last;
	int rear;
	T* data;
public:
	cq(void);
	cq(int l);
	~cq(void);
	void Enqueue(T* buf,int l);
	void Dequeue(int l);
	T* front(int l);
	int size();
	int FreeSpace();
	void dump();
	void set_capacity(int n);
	void clear();
};

template<class T>
void cq<T>::clear()
{
	first = 0;
	rear = 0;
}

template<class T>
void cq<T>::set_capacity(int n)
{
	T* tmp_data = new T[n+1];
	memset(tmp_data,0,sizeof(T)*(n+1));
	memcpy(tmp_data,data,sizeof(T)*(len>n?n:len));
	delete [] data;
	data = tmp_data;
	len = n;
	first = 0;
	rear = 0;
}

template<class T>
int cq<T>::FreeSpace()
{
	return (len - size());
}

template<class T>
T* cq<T>::front(int l)
{
	if (l>size()) return NULL;
	if (size() == 0) return NULL;
	return &data[first];
}

template<class T>
void cq<T>::dump()
{
	printf("--------------------\n");
	printf("len: %d, size: %d, first: %d, rear: %d\n",len,size(),first,rear);
	for (int i = first; i <rear; i++)
	{
		printf("%d  ",data[i]);
	}
	printf("--------------------\n");
}

template<class T>
void cq<T>::Dequeue(int l)
{
	int cnt = size();
	if (0 == cnt) return;
	if (l >= cnt)
	{
		first = 0;
		rear = 0;
		return;
	}
	first+= l;
	if(first == rear)
	{
		first = 0;
		rear = 0;
	}

}
#include <string>
extern std::string tohex(const char* buf,int nlen);
template<class T>
void cq<T>::Enqueue(T* buf,int l)
{
	int cnt = size();
	if(l >= len)
	{
		memmove(data,&buf[l-len],len*sizeof(T));
		first = 0;
		rear = len;
		return;
	}
	if(rear+l > len)
	{
		memmove(data,&data[first],cnt*sizeof(T));
		first = 0;
		rear = cnt;
	}
	if(l+ cnt>len)
	{
		//LOG_INFO("溢出: capacity: %d, first: %d, rear: %d, new: %d",len,first,rear,l);
		//LOG_INFO("%s",tohex((char*)&data[first],cnt).c_str());
		int remain = len - l;
		memmove(data,&data[first+cnt-remain],remain*sizeof(T));
		first = 0;
		rear = remain;
	}
	memmove(&data[rear],buf,l*sizeof(T));
	rear += l;
}

template<class T>
int cq<T>::size()
{
	// 	if(0== (rear-first)%len)
	// 	{
	// 		if(-1 == last) return 0;
	// 		else return len;
	// 	}
	return  (rear-first) ;

}

template<class T>
cq<T>::~cq(void)
{
	delete [] data;
}

template<class T>
cq<T>::cq(int l)
{
	len = l;
	data = new T[len+1];
	memset(data,0,sizeof(T)*(len+1));
	first = 0;
	rear = 0;
	//last = -1;
}

template<class T>
cq<T>::cq(void)
{
	len = DEF_QUEUE_LEN;
	data = new T[len+1];
	memset(data,0,sizeof(T)*(len+1));
// 	len = 0;
// 	data = NULL;
	first = 0;
	rear = 0;
	//last = -1;
}

#endif