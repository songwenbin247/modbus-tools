#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lib_thread_pool.h"
#include "errlog.h"

#define LOG_FILE "./log"

void * thread_1 (void *arg)
{
	printf("fun = %s thread_id = %u, arg = %d ", __func__, pthread_self(), (int)arg);
}

void * thread_2 (void *arg)
{
	printf("fun = %s thread_id = %u, arg = %d ", __func__, pthread_self(), (int)arg);
}


void * thread_3 (void *arg)
{
	printf("fun = %s thread_id = %u, arg = %d ", __func__, pthread_self(), (int)arg);
}

void * thread_4 (void *arg)
{
	printf("fun = %s thread_id = %u, arg = %d ", __func__, pthread_self(), (int)arg);
}


int main()
{
	if (log_open(LOG_FILE))
		return 0;

	if (!thread_pool_init(4)){
		goto close_log;
	}

	int i;
	for (i = 0; i < 10; i++){
		get_thread(thread_1, (void *)1);
		get_thread(thread_2, (void *)2);
		get_thread(thread_3, (void *)3);
		get_thread(thread_4, (void *)4);
	}


close_log:
	log_close();

	return 0;	
}
