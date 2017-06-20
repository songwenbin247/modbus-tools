#ifndef __ERR_LOG__H__
#define __ERR_LOG__H__
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
void Info_put(const char *format, ...);

#ifdef DEBUG
#define Debug_put(format, ...) __Debug_put(format, ##__VA_ARGS__)

#else
#define Debug_put(format, ...)  
#endif

void Err_put(const char *format, ...);
#endif
