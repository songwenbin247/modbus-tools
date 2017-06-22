#include <stdio.h>
#include <stdlib.h>
#include "lib_fd_pool.h"

#define DF_NAME test_fd

FD_POOL_DECLARE(DF_NAME);

int main()
{
	if (FD_POOL_INIT(DF_NAME, 10))
		return 0;
	int i = 0;
	int j;
	int k;
	struct fd_pool_t *fp;

		j = random();
	while (FD_POOL_INSERT(DF_NAME, j, NULL, (void *)(long)i) == 0){
		printf("add %d: fd = %d\n", i++, j);
		if (i == 6)
			k = j;
		j = random();	
	}

	FD_FOR_EACH(fp, DF_NAME){
		printf("loop: fd = %d : i = %d\n", fp->fd, (int)(long)fp->arg);
	}	
	
	printf("FIND %d\n", k);
	fp = FD_POOL_FIND(DF_NAME, k);

	if (fp)
		printf("Finded fp->fd = %d fp->arg = %p\n", fp->fd, fp->arg);
	
	FD_FOR_EACH(fp, DF_NAME){
		printf("del: fd = %d : i = %d\n", fp->fd, (int)(long)fp->arg);
		FD_POOL_DEL(DF_NAME, fp);
	}	
	printf("start loop after del\n");	
	FD_FOR_EACH(fp, DF_NAME){
		printf("loop: fd = %d : i = %d\n", fp->fd, (int)(long)fp->arg);
	}	
	printf("end\n");

	FD_POOL_FREE(DF_NAME);
	return 0;
}
// Test result
//gcc test_fd_pool.c lib_fd_pool.c rbtree.c
//./a.out
//add 0: fd = 1804289383
//add 1: fd = 846930886
//add 2: fd = 1681692777
//add 3: fd = 1714636915
//add 4: fd = 1957747793
//add 5: fd = 424238335
//add 6: fd = 719885386
//add 7: fd = 1649760492
//add 8: fd = 596516649
//add 9: fd = 1189641421
//loop: fd = 424238335 : i = 5
//loop: fd = 596516649 : i = 8
//loop: fd = 719885386 : i = 6
//loop: fd = 846930886 : i = 1
//loop: fd = 1189641421 : i = 9
//loop: fd = 1649760492 : i = 7
//loop: fd = 1681692777 : i = 2
//loop: fd = 1714636915 : i = 3
//loop: fd = 1804289383 : i = 0
//loop: fd = 1957747793 : i = 4
//FIND 424238335
//Finded fp->fd = 424238335 fp->arg = 0x5
//del: fd = 424238335 : i = 5
//del: fd = 596516649 : i = 8
//del: fd = 719885386 : i = 6
//del: fd = 846930886 : i = 1
//del: fd = 1189641421 : i = 9
//del: fd = 1649760492 : i = 7
//del: fd = 1681692777 : i = 2
//del: fd = 1714636915 : i = 3
//del: fd = 1804289383 : i = 0
//del: fd = 1957747793 : i = 4
//start loop after del
//end
//
