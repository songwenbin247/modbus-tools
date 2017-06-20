#ifndef __LIB___THERAD_POOL_____H__
#define __LIB___THERAD_POOL_____H__
#include "list.h"

typedef void *(*thread_fn)(void *);

/******************/
//  tidp: Pthread id, Set when the pthread was created. 
//  fn: Function, Will be executed after pthread_cond_signal(), and set by user.
//  id: identify this struction. 
//  cond: Communicates with pthread. 
//  count: enables the execution of fn.
//  mutex: mutex, protect the fields of this struction.
//  tp_list: Managed by LIB_PRI_MALLOC lib.
//  
/******************/
struct thread_pool_t
{
	pthread_t tidp;
	thread_fn fn;
	int id;
	pthread_cond_t cond;
	int count;  // if count == SHUT_DOWN, this thread will exit after pthread_cond_signal()    
	pthread_mutex_t mutex; 
	void *arg;
	struct list_head tp_list;
};

typedef  struct thread_pool_t * tp_t;

int thread_pool_init(int threads_num);
tp_t get_thread(thread_fn fn, void *arg);
int kill_thread(tp_t tp);
int wait_thread_over(tp_t tp);
tp_t __find_an_over_thread(void);
tp_t find_an_over_thread(void);
void put_thread(tp_t tp);
void thread_pool_free(void);

#define __get_over_info(tp) (tp->count)
#define GET_ID(tp) (tp->id)
#endif
