#include "lib_pri_malloc.h"
#include "rbtree.h"
#include "list.h"
#include "lib_thread_pool.h"
#include "lib_fd_pool.h"

int  __fd_pool_insert(struct fd_pool_t *fp, int fd, thread_fn fn, void *arg, struct rb_root *root)
{
	struct rb_node **new = &root->rb_node;
	struct rb_node *parent = NULL;
	
	if (fp == NULL)
		return -1;
	fp->fn = fn;
	fp->arg = arg;
	fp->fd = fd;

	while (*new){
		parent = *new;
		if (fd < rb_entry(parent, struct fd_pool_t, rb)->fd)
			new = &parent->rb_left;
		else
			new = &parent->rb_right;
	}
	rb_link_node(&fp->rb, parent, new);
	rb_insert_color(&fp->rb,root);
	return 0;
} 

void   __fd_pool_def(struct fd_pool_t *fp, struct rb_root *root){
	rb_erase(&fp->rb, root);	
}



struct fd_pool_t *  __fd_pool_find(int fd, struct rb_root *root)
{
	
	struct rb_node **new = &root->rb_node;
	struct rb_node *parent = NULL;
	int fd_r;
	int i = 0;
	while (*new){
		parent = *new;
		fd_r = rb_entry(parent, struct fd_pool_t, rb)->fd;
		if (fd < fd_r){
			new = &parent->rb_left;
		}
		else if (fd > fd_r ){
			new = &parent->rb_right;
		}
		else{
			break;
		}
	}
	if (!*new)
		return NULL;
	return rb_entry(*new, struct fd_pool_t, rb);
}
