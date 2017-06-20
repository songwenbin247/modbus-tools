#include <stdio.h>
#include <stdarg.h>
#include "errlog.h"
#define LOG_FILE "./log"
int main()
{
	if (log_open(LOG_FILE)){
		fprintf(stderr, "Fail to open log file %s\n", LOG_FILE);
		return 0;
	}
	Info_put("look, world %d\n", 10);
	Debug_put("look, world %d\n",11);
	Err_put("look, world %d\n", 12);
	log_close();
	return 0;
}

//test result:
//gcc -DDEBUG test_errlog.c errlog.c
//INfo: look, world 10
//Debug:look, world 11
//Err:  look, world 12
//
//gcc  test_errlog.c errlog.c
//INfo: look, world 10
//Err:  look, world 12
//
//

