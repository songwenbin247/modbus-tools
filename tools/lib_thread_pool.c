#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include "lib_pri_malloc.h"
#include "lib_thread_pool.h"
#include "list.h"
#include "errlog.h"

#define SHUT_DOWN INT_MAX
#define POOL_NAME thread_pool
PRI_DECLARE(struct thread_pool_t, POOL_NAME);
pthread_rwlock_t tp_rwlock;

static void *load_thread_fn(void *arg)
{
	struct thread_pool_t *tp = arg;
	int ret = 0;
	
	Info_put("thread_id = %u: Start.\n", tp->id);
	pthread_mutex_lock(&tp->mutex);

	for (;;){
		
		while (tp->count < 1)
			pthread_cond_wait(&tp->cond, &tp->mutex);
			
		if (tp->count == SHUT_DOWN){
			Info_put("thread_id = %u: Get SHUT_DOWN signal.\n", tp->id);
			break;	
		}
		
		Debug_put("thread_id = %u: start fn\n", tp->id);
		(*tp->fn)(tp->arg);
		Debug_put("thread_id = %u: end fn\n", tp->id);
		tp->count--;

		if (tp->count == 0)
			put_thread(tp);	
	}
	
	pthread_mutex_unlock(&tp->mutex);
	Info_put("thread_id = %u: End.\n", tp->id);
	pthread_exit((void *)ret);
}

int thread_pool_init(int threads_num)
{
	struct thread_pool_t *tp;
	int ret;
	int id = 1;
//	int core_num = sysconf(_SC_NPROCESSORS_ONLN);
	if ((ret = PRI_MALLOC(POOL_NAME, threads_num, tp_list)) < 0){
		Err_put("%s:PRI_MALLOC error.\n", __func__);
		return ret;
	}
	
	if ((ret = pthread_rwlock_init(&tp_rwlock, NULL)) != 0){
		Err_put("%s:rwlock_init error.\n", __func__);
		goto rwlock_fail; 
	}

	pthread_rwlock_wrlock(&tp_rwlock);

	PRI_LIST_FOR_UNUSED(tp, POOL_NAME, tp_list){
		tp->id = id++;		
		pthread_mutex_init(&tp->mutex, NULL);
		
		if ((ret = pthread_cond_init(&tp->cond, NULL)) < 0){
			Err_put("%s:pthread_cond_init error.\n", __func__);
			goto cond_fail;
		}
		tp->count = 0;
		ret = pthread_create(&tp->tidp, NULL, load_thread_fn, (void*)tp);
		if (ret != 0){
			Err_put("%s:pthread_create error.\n", __func__);
			goto create_fail;
		}				
	}
	return ret;
create_fail:
	pthread_cond_destroy(&tp->cond);
cond_fail:

	pthread_mutex_destroy(&tp->mutex);
	
	PRI_LIST_FOR_UNUSED(tp, POOL_NAME, tp_list){
		if (tp->id != 0 && tp->id < id - 1){
			kill_thread(tp);
			pthread_cond_destroy(&tp->cond);
			pthread_mutex_destroy(&tp->mutex);
		}
	}	

	pthread_rwlock_unlock(&tp_rwlock);
	pthread_rwlock_destroy(&tp_rwlock);

rwlock_fail:	
	PRI_FREE(POOL_NAME);
	return ret;
}

tp_t get_thread(void * (*fn)(void *), void *arg)
{
	pthread_rwlock_wrlock(&tp_rwlock);
	tp_t tp = PRI_GET(POOL_NAME, tp_list);
	if (!tp){
		Err_put("%s:PRI_GET isnot enough.\n", __func__);
		return NULL;
	}
	
	pthread_mutex_lock(&tp->mutex);
	pthread_rwlock_unlock(&tp_rwlock);

	tp->fn = fn;
	tp->arg = arg;
	tp->count++;
	pthread_cond_signal(&tp->cond);
	pthread_mutex_unlock(&tp->mutex);
	
	return tp;	
}

int kill_thread(tp_t tp)
{
	pthread_mutex_lock(&tp->mutex);
	tp->count = SHUT_DOWN;
	pthread_cond_signal(&tp->cond);
	pthread_mutex_unlock(&tp->mutex);
	pthread_join(tp->tidp, NULL);
}

int wait_thread_over(tp_t tp)
{
	int ret;
	for (;;){
		pthread_mutex_lock(&tp->mutex);
		ret = tp->count;
		pthread_mutex_unlock(&tp->mutex);
		if (ret == 0)
			return 0;
		usleep(1000);
	}
	return 0;
}

void put_thread(tp_t tp)
{
	pthread_rwlock_wrlock(&tp_rwlock);
	PRI_DEL(tp, POOL_NAME, tp_list);
	pthread_rwlock_unlock(&tp_rwlock);

}


int get_over_info(tp_t tp)
{
	int ret;
	pthread_mutex_lock(&tp->mutex);
	ret = __get_over_info(tp);
	pthread_mutex_unlock(&tp->mutex);
	return ret;
}

tp_t __find_an_over_thread()
{
	tp_t tp;
	int ret, find = 0;
	PRI_LIST_FOR_BUSY(tp, POOL_NAME, tp_list){
		if (pthread_mutex_trylock(&tp->mutex) != 0)
			continue;
		ret = tp->count;
		pthread_mutex_unlock(&tp->mutex);
		if (ret == 0){
			find = 1;
			break;
		}				
	}
	return find ? tp : NULL;
}

tp_t inline find_an_over_thread()
{
	tp_t tp;
	pthread_rwlock_rdlock(&tp_rwlock);
	tp = __find_an_over_thread();
	pthread_rwlock_unlock(&tp_rwlock);
	return tp;
}


void thread_pool_free()
{	
	tp_t tp;
	pthread_rwlock_wrlock(&tp_rwlock);
	
	PRI_LIST_FOR_BUSY(tp, POOL_NAME, tp_list){
		PRI_DEL(tp, POOL_NAME, tp_list);
	}	
	
	PRI_LIST_FOR_UNUSED(tp, POOL_NAME, tp_list){
		kill_thread(tp);
		pthread_cond_destroy(&tp->cond);
		pthread_mutex_destroy(&tp->mutex);
	}
	
	pthread_rwlock_unlock(&tp_rwlock);

	PRI_FREE(POOL_NAME);
	pthread_rwlock_destroy(&tp_rwlock);
}

