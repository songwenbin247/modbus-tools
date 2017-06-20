#include <stdio.h>
#include <pthread.h>
#include <stdarg.h>
#include "errlog.h"

FILE *debug_file_desc;
pthread_mutex_t debug_mutex;

int log_open(const char *file)
{
	if ((debug_file_desc = fopen(file,"w")) == NULL){
		fprintf(stderr, "Fail to open log file %s\n", file);
		return 1;	
	}
	pthread_mutex_init(&debug_mutex, NULL);
	return 0;
}

void log_close()
{
	fclose(debug_file_desc);
	pthread_mutex_destroy(&debug_mutex);
}

void Info_put(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	pthread_mutex_lock(&debug_mutex);
	fprintf(debug_file_desc,"%s", "INfo: ");
	vfprintf(debug_file_desc,format,ap);
	pthread_mutex_unlock(&debug_mutex);
	va_end(ap);
}

void __Debug_put(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	pthread_mutex_lock(&debug_mutex);
	fprintf(debug_file_desc,"%s", "Debug:");
	vfprintf(debug_file_desc,format,ap);
	pthread_mutex_unlock(&debug_mutex);
	va_end(ap);
}

void Err_put(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	pthread_mutex_lock(&debug_mutex);
	fprintf(debug_file_desc,"%s", "Err:  ");
	vfprintf(debug_file_desc,format,ap);
	pthread_mutex_unlock(&debug_mutex);
	va_end(ap);
}
