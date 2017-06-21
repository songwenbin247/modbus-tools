#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lib_thread_pool.h"
#include "errlog.h"

#define LOG_FILE "./log"

void * thread_1 (void *arg)
{
	printf("fun = %s thread_id = %u, arg = %d \n", __func__, (int)pthread_self(), (int)(long)arg);
}

void * thread_2 (void *arg)
{
	printf("fun = %s thread_id = %u, arg = %d \n", __func__, (int)pthread_self(), (int)(long)arg);
}


void * thread_3 (void *arg)
{
	printf("fun = %s thread_id = %u, arg = %d \n", __func__, (int)pthread_self(), (int)(long)arg);
}

void * thread_4 (void *arg)
{
	printf("fun = %s thread_id = %u, arg = %d \n", __func__, (int)pthread_self(), (int)(long)arg);
}


int main()
{
	if (log_open(LOG_FILE))
		return 0;

	if (thread_pool_init(4)){
		goto close_log;
	}

	int i;
	tp_t tp1, tp2, tp3, tp4;
	for (i = 0; i < 3; i++){
		tp1 = get_thread(thread_1, (void *)1);
		tp2 = get_thread(thread_2, (void *)2);
		tp3 = get_thread(thread_3, (void *)3);
		tp4 = get_thread(thread_4, (void *)4);
		wait_thread_over(tp1);
		wait_thread_over(tp2);
		wait_thread_over(tp3);
		wait_thread_over(tp4);

	}

	thread_pool_free();
close_log:
	log_close();

	return 0;	
}
//Test result:
// gcc test_thread_pool.c lib_thread_pool.c errlog.c -lpthread -DDEBUG
//songwb@OptiPlex:~/modbus_simulator/libmodbus-3.1.4/tools$ ./a.out
//fun = thread_1 thread_id = 3361801984, arg = 1
//fun = thread_2 thread_id = 3353409280, arg = 2
//fun = thread_4 thread_id = 3336623872, arg = 4
//fun = thread_3 thread_id = 3345016576, arg = 3
//fun = thread_4 thread_id = 3353409280, arg = 4
//fun = thread_3 thread_id = 3361801984, arg = 3
//fun = thread_2 thread_id = 3336623872, arg = 2
//fun = thread_1 thread_id = 3345016576, arg = 1
//fun = thread_4 thread_id = 3353409280, arg = 4
//fun = thread_2 thread_id = 3336623872, arg = 2
//fun = thread_3 thread_id = 3361801984, arg = 3
//fun = thread_1 thread_id = 3345016576, arg = 1
//songwb@OptiPlex:~/modbus_simulator/libmodbus-3.1.4/tools$ cat log
//INfo: thread_id = 1: Start.
//Debug:thread_id = 1: start fn
//INfo: thread_id = 2: Start.
//Debug:thread_id = 2: start fn
//INfo: thread_id = 4: Start.
//Debug:thread_id = 4: start fn
//Debug:thread_id = 2: end fn
//Debug:thread_id = 1: end fn
//INfo: thread_id = 3: Start.
//Debug:thread_id = 3: start fn
//Debug:thread_id = 4: end fn
//Debug:thread_id = 3: end fn
//Debug:thread_id = 2: start fn
//Debug:thread_id = 2: end fn
//Debug:thread_id = 4: start fn
//Debug:thread_id = 1: start fn
//Debug:thread_id = 1: end fn
//Debug:thread_id = 3: start fn
//Debug:thread_id = 4: end fn
//Debug:thread_id = 3: end fn
//Debug:thread_id = 2: start fn
//Debug:thread_id = 2: end fn
//Debug:thread_id = 4: start fn
//Debug:thread_id = 3: start fn
//Debug:thread_id = 4: end fn
//Debug:thread_id = 1: start fn
//Debug:thread_id = 1: end fn
//Debug:thread_id = 3: end fn
//INfo: thread_id = 3: Get SHUT_DOWN signal.
//INfo: thread_id = 3: End.
//INfo: thread_id = 1: Get SHUT_DOWN signal.
//INfo: thread_id = 1: End.
//INfo: thread_id = 4: Get SHUT_DOWN signal.
//INfo: thread_id = 4: End.
//INfo: thread_id = 2: Get SHUT_DOWN signal.
//INfo: thread_id = 2: End.
