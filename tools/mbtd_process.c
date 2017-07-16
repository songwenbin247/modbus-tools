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
#define LISTS_NUM  8
#define CONNECT_NUM 12
#define THREADS_NUM 16

FD_L_POOL_DECLARE(LISTS_NAME);
FD_L_POOL_DECLARE(CONNECT_NAME);

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
		goto free_fd_lc_p;
	}
	
	if ((ret = listen(fd, 5)) < 0){
		 Err_put("Can't listen Unix socket noblock\n")
		goto free_fd_lc_p;
	}

	memset(lists_fds, 0 , sizeof(lists_fds))
	if ((fp = FD_L_GET(LISTS_NAME)) == NULL){
		Err_put("Can't get fd list entry\n");
		goto free_fd_lc_p;
	}

	lists_fds[fp->id].fd = fd;
	lists_fds[fp->id].events = POLLIN;
	fp->fd = fd;
	fp->data = NULL;
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

free_fd_lc_p:
	FD_L_FREE(LISTS_NAME);
free_thead_p:
	 thread_pool_free();
	 return -1;
}

#define ADD_CONN  1
#define REMOVE_CONN 0
int connect_f[2];
pthread_rwlock_t connect_l;

void *connect_poll(void *arg)
{
	int rc;
	struct pollfd connect_fds[CONNECT_NUM + 1];
	if ((rc = pthread_rwlock_init(&connect_l, NULL)) != 0){
		goto err;
	}

	if (pipe(connect_f)){
		goto free_rwlock;
	}
	
	if (FD_L_POOL_INIT(CONNECT_NAME, CONNECT_NUM) < 0){
		 goto close_f;
	}
	
	connect_fds[CONNECT_NUM].fd = connect_f[0];
        connect_fds[CONNECT_NUM].events = POLLIN;	
	
	do{
		if ((ret = poll(connect_fds, CONNECT_NUM + 1, -1)) < 0){
			goto free_pool;
		}

		if (ret < 0){
			goto free_pool;
		}

		for (i = 0; i < CONNECT_NUM + 1, i++){
			if (connect_fds[i].revents == 0)
				continue;
			if (connect_fds[i].revents != POLLIN)
				continue;
			if (connect_fds[i].fd != connect_f[0])
				FD_L_LIST_FOR_BUSY(fp, CONNECT_NAME){
					if (fp->fd == connect_fds[i].fd){
						fp->fn(fp);
						break;
					}
				}	
			}
			else{
				char buf[8];
				int len;
				struct fd_pool_t *fpc;
				int fd = 0, i;
				pthread_rwlock_rdlock(&connect_l);
				len = read(fd[1], buf, 8);
				pthread_rwlock_unlock(&connect_l);
				for (i = 0; i < sizeof(int); i++){
					fd += buf[i] << (8 * i); 
				}

				if (buf[sizeof(int)] == ADD_CONN){
					if ((fpc = FD_L_GET(CONNECT_NAME)) < 0){
						Err_put("Can't get %s entry\n", CONNECT_NAME);
						continue;
					}
					fpc->fd = fd;
					connect_fds[fpc->id].fd = fd;
					connect_fds[fpc->id].events = POLLIN;
					fpc->fn = process_connect;
				}
				else if (buf[sizeof(int)] == REMOVE_CONN){
					int find = 0;
					FD_L_LIST_FOR_BUSY(fp, CONNECT_NAME){
						if (fp->fd == fd){
							find = 1;
							break;
						}
					}
					if (find){
						connect_fds[fpc->id].fd = 0;
						FD_L_DEL(fpc, CONNECT_NAME);
					}
				}
				else{
					goto free_pool;
				}


			}
		}
	}
	
free_pool:
	FD_L_FREE(CONNECT_NAME);

close_f:
	close(connect_f[0]);
	close(connect_f[1]);

free_rwlock:
	pthread_rwlock_destroy(&connect_l);
err:
	Err_put("Connect poll failed\n");
}

void poll_connect(int fd, int how)
{
	char buf[8];
	buf[sizeof(int)] = how; // add
	int i;
	for (i = 0; i < sizeof(int)){
		buf[i] = (char)(fd & 0Xff);	
		fd >>= 8;
	}
	pthread_rwlock_wrlock(&connect_l);
	write(connect_f[1], buf, sizeof(int) + 1);
	pthread_rwlock_unlock(&connect_l);
}

void connect_socket(void *arg)
{
	struct fd_pool_t *fp = arg;
	struct sockaddr_in client_addr;
	int fd;	
	
	fd = accept(arg->fd, &client_addr, sizeof(client_addr));

	if (fd < 0){
		Err_put("Can't accept a client connection\n");
		return;
	}

	Info_put("Accept: ip: %s, port: %d\n", inet_ntoa(client_addr.sin_addr), ntoha(client_addr.sin_port));
	poll_connect(fd, ADD_CONN);	

}
void create_listen(char buf, struct pollfd *fds)
{
	struct fd_pool_t *fp;
	struct sockaddr_in *self
	int ret;

	if ((fp = FD_L_GET(LISTS_NAME)) == NULL){
		Err_put("Can't get fd list entry\n");
		goto err;
	}
	
	if ((fp->fd = socket(AF_NET, SOCK_STREAM, 0)) < 0){
		Err_put("Can't create a socket\n");
		goto free_fp;	
	}
	
	if ((self = malloc(sizeof( struct sockaddr_in ))) < 0){
		Err_put("Can't malloc memory for a sockaddr\n");
		goto free_fp;
	}

	bzero(&self, sizeof(self));
	self->sin_family = AF_INET;
	self->sin_port = *((uint_16_t)(buf + 4));
	self->sin_addr.s_addr = *((in_addr_t *)buf);
	fp->data = self;
	
	if (ioctl(fp->fd, FIONBIO, NULL) < 0){
		Err_put("Can't set socket noblock\n");
		goto free_self;
	}
	

	if (bind(fp->fd, (struct sockaddr*)self, sizeof(*self)) != 0 ){
		Err_put("Can't bind a socket\n");
		goto free_self;
	}
	
	if (listen(fd, 5) < 0){
		 Err_put("Can't listen  socket\n")
		goto free_self;
	}
	
	fds[fp->id].fd = fd;
	fds[fp->id].events = POLLIN;
	
	buf[0] = 1;
	strcpy(&buf[1], "create this listen successfully\n");
	return;

free_self:
	free(self);
free_fp:
	FD_L_DEL(fp, LISTS_NAME);
err:
	buf[0] = 0;
	strcpy(&buf[1], "Can't create this listen\n");
}

void close_listen(char buf, struct pollfd *fds)
{
	struct fd_pool_t *fp;
	int find = 0;
	FD_L_LIST_FOR_BUSY(fp, LISTS_NAMER){
		if (fp->fd == buf[0]){
			find = 1;
			break;
		}
	}
	
	if (!find){
		buf[0] = 0;
		strcpy(&buf[1], "Can't find this listen\n");
		return;
	}
	
	fds[fp->id].fd = 0;
	close(fp->fd);
	free(fp->data);
	FD_L_DEL(fp, LISTS_NAME)	
	buf[0] = 1;
	strcpy(&buf[1], "has deleted this listen\n");
	
}
void list_listen(char buf)
{
	struct fd_pool_t *fp;
	int find = 0;
	FD_L_LIST_FOR_BUSY(fp, LISTS_NAMER){
		if (fp->fd == buf[0]){
			find = 1;
			break;
		}
	}
	
	if (!find){
		buf[0] = 0;
		strcpy(&buf[1], "Can't find this listen\n");
		return;
	}
	
	buf[0] = 1;
	strcpy(&buf[1], "ID = %d: IP = %s PORT = %d\n", fp->fd, inet_ntoa(fp->data->sin_addr), ntoha(fp->data->sin_port));
	
	
}
void list_listens(char buf)
{
	struct fd_pool_t *fp;
	int i = 1;
	
	buf[0] = 0;
	FD_L_LIST_FOR_BUSY(fp, LISTS_NAMER){
		buf[0] = 1;
		sprintf(&buf[i], "ID = %d: IP = %s PORT = %d\n",
			fp->fd, inet_ntoa(fp->data->sin_addr), ntoha(fp->data->sin_port));

		i = strlen(&buf[1]) + 1;
	}	
}
	
void create_dev(char buf)
void close_dev(char buf)
void list_dev(char buf)
void list_devs(char buf)
void run_config(char buf)
void save_config(char buf)
void process_mbt(int fd, struct pollfd *fds)
{
	char buff[COMMAND_DATA_MAX];
	int len;
	len = recv(fd, buf, COMMAND_DATA_MAX, MSG_WAITALL);
	char *buf = buff;
	buf++;
	
	switch(bufif[i]){
		case C_LIST_ID:
	       		create_listen(buf, fds); 	
			break;
		case L_LISTS_ID:
			list_listens(buf);
			break;
		case L_LIST_ID:
			list_listen(buf);
			break;
		case O_LIST_ID:
			close_listen(buf, fds);
			break;
		case C_DIV_ID:
			create_dev(buf);
			break;
		case L_DIV_ID:
		       list_dev(buf);
	       		break;
		case L_DIVS_ID;
 			list_devs(buf);
			break;
		case O_DIV_ID:
			close_dev(buf);
			break;
		case R_FILE_ID:		
			run_config(buf);
			break;
		case S_FILE_ID:
			save_config(buf);
			break;
		default:
			buf[0] = 0;
			strcpy(&buf[1], "Don't support\n", 14);
	}	
	send(fd, buf, strlen(buf), MSG_DONTROUTE);
}


