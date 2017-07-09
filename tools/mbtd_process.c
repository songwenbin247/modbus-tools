#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <limits.h>
#include <netinet/in.h>
#include <sys/poll.h>

#include "mbt.h"
#include "lib_fd_pool.h"
#include "lib_thread_pool.h"

#define LISTS_NAME listener
#define LISTS_NUM  16

#define THREADS_NUM 32

FD_L_POOL_DECLARE(LISTS_NAME);

void process_mbt(int fd, struct pollfd *fds)
{

}

int start_process(int fd)
{
	int ret;	
	struct fd_pool_t *fp;
	struct pollfd lists_fds[LISTS_NUM];
	if (thread_pool_init(THREADS_NUM) < 0){
		Err_put("Can't create thread pool\n");
		return -1;
	}

	if (FD_L_POOL_INIT(LISTS_NAME, LISTS_NUM) < 0){
		Err_put("Can't create FD List pool\n");
		 goto free_thead_p;
	}
	
	if ((ret = ioctl(fd, FIONBIO, NULL)) < 0){
		Err_put("Can't set Unix socket noblock\n");
		goto free_fd_l_p;
	}
	
	if ((ret = listen(fd, 5)) < 0){
		 Err_put("Can't listen Unix socket noblock\n")
		goto free_fd_l_p;
	}

	memset(lists_fds, 0 , sizeof(lists_fds))
	if ((fp = FD_L_GET(LISTS_NAME)) == NULL){
		Err_put("Can't get fd list entry\n");
		goto free_fd_l_p;
	}

	lists_fds[fp->id].fd = fd;
	lists_fds[fp->id].events = POLLIN;
	fp->fd = fd;

	do{
		if ((ret = poll(lists_fds, LISTS_NUM, -1)) < 0){
			goto free_fd_l_p;
		}

		for (i = 0; i < LISTS_NUM, i++){
			if (lists_fds[i].revents == 0)
				continue;
			if (lists_fds[i].fd != fd){
				FD_L_LIST_FOR_BUSY(fp, LISTS_NAME){
					if (fp->fd == lists_fds[i].fd){
						fp->fn(fp);
						break;
					}
				}	
			}
			else if (lists_fds[i].fd == fd){
				process_mbt(fd, lists_fds);
			}
		}
	}

free_fd_l_p:
	FD_L_FREE(LISTS_NAME);
free_thead_p:
	 thread_pool_free();
	 return -1;
}
