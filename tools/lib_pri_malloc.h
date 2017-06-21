#ifndef __PRI_MALLOC__H__
#define __PRI_MALLOC__H__
#include <stdlib.h>
#include  "list.h"
#include <string.h>
struct pri_list_head
{
	struct list_head busy;
	struct list_head unused;
	int sum;
};

#define PRI_HEAD(name)  name##_pri_list_head
#define PRI_HEAD_P(name) name##_pri_list_head_p

#define PRI_DECLARE(type, name_p)   \
		type *name_p; \
		struct pri_list_head *PRI_HEAD_P(name_p)

#define PRI_INIT(name, member) { \
		int i; \
		for (i = 0; i < PRI_HEAD_P(name)->sum; i++){\
			INIT_LIST_HEAD(&name[i].member);\
			list_add(&name[i].member, &PRI_HEAD_P(name)->unused);\
		} \
	}

#define PRI_MALLOC(name_p, num, member)  ({\
		name_p = malloc(num * sizeof(*name_p)); \
		memset(name_p, 0 , num * sizeof(*name_p));\
		int ret = -1; \
		if (name_p){ \
			PRI_HEAD_P(name_p) = malloc(sizeof(struct pri_list_head)); \
			if (!PRI_HEAD_P(name_p))\
				free(name_p);\
			else {\
				ret = 0;\
				INIT_LIST_HEAD(&PRI_HEAD_P(name_p)->busy);\
				INIT_LIST_HEAD(&PRI_HEAD_P(name_p)->unused);\
				PRI_HEAD_P(name_p)->sum = num; \
				PRI_INIT(name_p,member);\
			}\
		} \
		ret; \
		})

#define PRI_FREE(name)\
			free(name);\
			free(PRI_HEAD_P(name))\


#define PRI_GET(name, member) ({ \
		struct list_head *temp; \
		typeof(name[0]) *ref; \
		if (list_empty(&PRI_HEAD_P(name)->unused)) \
			ref = NULL;\
		else{\
			temp = PRI_HEAD_P(name)->unused.next; \
			list_move(PRI_HEAD_P(name)->unused.next, &PRI_HEAD_P(name)->busy); \
			ref = container_of(temp, typeof(name[0]), member); \
		}\
		ref; \
		})

#define PRI_LIST_FOR_BUSY(temp, name, member) \
		for (temp = container_of(PRI_HEAD_P(name)->busy.next, typeof(name[0]), member);\
		   temp->member.next != &PRI_HEAD_P(name)->busy; \
		   temp = container_of(temp->member.next, typeof(name[0]), member) ) 

#define PRI_LIST_FOR_UNUSED(temp, name, member) \
		for (temp = container_of(PRI_HEAD_P(name)->unused.next, typeof(name[0]), member);\
		   temp->member.next != &PRI_HEAD_P(name)->unused; \
		   temp = container_of(temp->member.next, typeof(name[0]), member) ) 

#define PRI_LIST_FOR_BUSY_SAFE(temp, name, member, n) \
		for (temp = container_of(PRI_HEAD_P(name)->busy.next, typeof(name[0]), member), \
			n = temp->member.next;\
		   temp->member.next != &PRI_HEAD_P(name)->busy; \
		   temp = container_of(n, typeof(name[0]), member),\
		   n = temp->member.next) 


#define PRI_DEL(pname, name, member) {\
		list_move(&(pname)->member, &PRI_HEAD_P(name)->unused); \
		}

#define PRI_COM_DEL(name, member, compare)({\
		typeof(name[0]) *ref; \
		struct list_head *n; \
		PRI_LIST_FOR_BUSY_SAFE(ref, name, member, n) {\
			if (compare(ref) == 0){ \
				PRI_DEL(ref, name, member);} \
		}\
		})


#endif
