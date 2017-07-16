#ifndef _____LIB_FD_POOL____H___
#define  _____LIB_FD_POOL____H___

#include "lib_pri_malloc.h"
#include "rbtree.h"
#include "list.h"
#include "lib_thread_pool.h"

struct fd_pool_t
{
	int fd;
	int id;
	struct rb_node rb;
	struct list_head list;
	thread_fn fn;	
	void *data;
};

#define FD_L_NAME(name) name##_FD_L

#define FD_L_POOL_DECLARE(name) \
	PRI_DECLARE(struct fd_pool_t, name);\


#define FD_L_POOL_INIT(name, num) ({\
	PRI_MALLOC(name, num, list) \
	struct fd_pool_t *tem; \
	int i = 0;\
	PRI_LIST_FOR_UNUSED(tem, name, list)\
		tem->id = i++; })
	

#define FD_L_POOL_FREE(name) \
	PRI_FREE(name)

#define FD_L_GET(name) \
	PRI_GET(name, list)

#define FD_L_DEL(entry, name) \
	PRI_DEL(entry, name. list)
#define FD_L_COM_DEL(name, compare) \
	PRI_COM_DEL(name, list, compare)

#define FD_L_LIST_FOR_BUSY(temp, name) \
	PRI_LIST_FOR_BUSY(temp, name, list)

#define FD_L_LIST_FOR_UNUSED(temp, name) \
	PRI_LIST_FOR_UNUSED(temp, name, list)

#define FD_L_LIST_FOR_BUSY_SAFE(temp, ntemp, name) \
	PRI_LIST_FOR_BUSY_SAFE(temp, name, list, ntemp)




#define RB_NAME(name) name##_RB_ROOT

#define FD_POOL_DECLARE(name) \
	PRI_DECLARE(struct fd_pool_t, name);\
	struct rb_root RB_NAME(name) = RB_ROOT	

#define FD_POOL_INIT(name, num) ({ \
	PRI_MALLOC(name, num, list) \
	struct fd_pool_t *tem; \
	int i = 0;\
	PRI_LIST_FOR_UNUSED(tem, name, list)\
		tem->id = i++; })

#define FD_POOL_FREE(name) \
	PRI_FREE(name)
#define FD_POOL_INSERT(name, fd, fn, arg) ({\
		struct fd_pool_t *fp = PRI_GET(name, list); \
		struct rb_root *root = &RB_NAME(name); \
		__fd_pool_insert(fp, fd, fn, arg, root);})

int __fd_pool_insert(struct fd_pool_t *fp, int fd, thread_fn fn, void *arg, struct rb_root *root);

#define FD_POOL_DEL(name, fp) ({\
		struct rb_root *root = &RB_NAME(name); \
		__fd_pool_def(fp, root);\
		PRI_DEL(fp, name, list);})
	

void __fd_pool_def(struct fd_pool_t *fp, struct rb_root *root);

#define FD_POOL_FIND(name, fd) ({\
	struct rb_root *root =  &RB_NAME(name);\
		__fd_pool_find(fd, root);})

struct fd_pool_t * __fd_pool_find(int fd, struct rb_root *root);

#define FD_FIRST(pos, name) ({\
		struct rb_node *rb; \
		rb = rb_first(&RB_NAME(name));\
		if (rb) \
			pos = rb_entry(rb,typeof(*pos), rb);\
		else\
			pos = NULL; \
		pos;})
#define FD_NEXT(pos, name) ({\
		struct rb_node *rb; \
		rb = rb_next(&pos->rb);\
		if (rb) \
			pos = rb_entry(rb,typeof(*pos), rb);\
		else\
			pos = NULL; \
		pos;})
	

#define FD_FOR_EACH(pos, name) \
	for (pos = FD_FIRST(pos, name); pos;\
		pos =  FD_NEXT(pos, name)) 



#endif
