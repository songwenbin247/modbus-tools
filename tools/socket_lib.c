#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/select.h>
#include <netinet/in.h>

#define LISTENERS_MAX  5
#define MASTER_DEVICE_MAX 5
#define UNIX_SOCKET_PATH "/var/modbus/modbus.socket"

struct client_t
{
	int status;
	int fd
};

struct listeners_t
{
	int status;  // 0 - unused;  1 -
	int fd;
	struct sockaddr_in sock_addr;	
	struct client_t client[MASTER_DEVICE_MAX];
};
struct unix_listener_t
{
	int status;
	int fd;
	struct sockaddr_un sock_addr;
	struct client_t client;
};

struct socket_info_t
{
	struct unix_listener_t unix_listener;
	struct listeners_t listeners[LISTENERS_MAX];
	fd_set read_set;
	fd_set write_set;
	fd_set except_set; 
	int maxfdp;
}socket_info;


int create_bind_unix_socket(char *socket_name)
{
	int fd, size, err;
	struct sockaddr_un un;
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path,socket_name);

	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		pri_err("unix socket create");
		return fd;
	}

	size = offsetof(struct sockaddr_un, sun_patch) + strlen(un.sun_path);

	if (bind(fd, (struct sockaddr *)&un, size) < 0){
		pri_err("unix socket bind");
		goto err_out;
	}
	
	return fd;
err_out:
	err = errno;
	close(fd);
	errno = err;
	return -1;
}


int socket_lib_init()
{
	int fd;
	
	memset(socket_info, 0, sizeof(socket_info));
	if ((fd = create_bind_unix_socket(UNIX_SOCKET_PATH)) < 0){
		return fd;
	}
	
	socket_info.unix_listener.fd = fd;
	socket_info.maxfdp = fd;
	FD_ZERO(&socket_info.read_set);
	FD_ZERO(&socket_info.write_set);
	FD_ZERO(&socket_info.except_set);




}






