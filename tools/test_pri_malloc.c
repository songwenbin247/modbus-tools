#include <stdio.h> 
#include <stdlib.h>
#include "lib_pri_malloc.h"
#include "list.h"
#include <stddef.h>
#include <stdlib.h>
struct test_struct {
	int index;
	struct list_head list_index;	
};

int com(struct test_struct *t)
{
	if (t->index == 3)
		return 0;
	return 1;	
}

#define STRUCT_NAME ptest_index

PRI_DECLARE(struct test_struct, STRUCT_NAME);

int dynamic_pri_malloc()
{
	return PRI_MALLOC(STRUCT_NAME, 10, list_index);
}

void free_pri_malloc()
{
	PRI_FREE(STRUCT_NAME);
}

int main()
{
	int i;
	struct test_struct *test;
	dynamic_pri_malloc();
	for (i = 0; i < 100; i++){
		if ( (test = PRI_GET(STRUCT_NAME, list_index)) != NULL){
			test->index = i;	
		}
		else{
			break;
		}
	}
	
	PRI_LIST_FOR_BUSY(test, STRUCT_NAME, list_index)
	{
		printf("BUSY: id = %d\n",test->index);
	}
	
	printf("-------------------------\n");
	PRI_COM_DEL(STRUCT_NAME, list_index, com);
	
	PRI_LIST_FOR_BUSY(test, STRUCT_NAME, list_index)
	{
		printf("BUSY: id = %d\n",test->index);
	}
	
	printf("-------------------------\n");
 
	PRI_DEL( STRUCT_NAME + 0, STRUCT_NAME, list_index);
	PRI_DEL( STRUCT_NAME + 1, STRUCT_NAME, list_index);
	PRI_DEL( STRUCT_NAME + 2, STRUCT_NAME, list_index);
	
	PRI_LIST_FOR_BUSY(test, STRUCT_NAME, list_index)
	{
		printf("BUSY: id = %d\n",test->index);
	}
	printf("-------------------------\n");

	free_pri_malloc();
	return 0;
}

//The output result:
//=================
//BUSY: id = 9
//BUSY: id = 8
//BUSY: id = 7
//BUSY: id = 6
//BUSY: id = 5
//BUSY: id = 4
//BUSY: id = 3
//BUSY: id = 2
//BUSY: id = 1
//-------------------------
//BUSY: id = 9
//BUSY: id = 8
//BUSY: id = 7
//BUSY: id = 6
//BUSY: id = 5
//BUSY: id = 4
//BUSY: id = 2
//BUSY: id = 1
//-------------------------
//BUSY: id = 6
//BUSY: id = 5
//BUSY: id = 4
//BUSY: id = 2
//BUSY: id = 1
//-------------------------
