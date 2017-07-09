#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <limits.h>
#define COMMAND_MAX  15
#define REQUEST_MAX  256
#define DOCUMENT ./README  
#define SOCKET_NAME "~/modbus_socket"

#define COMMAND_DATA_MAX 256

#define LOCKFILE "/var/run/mbtd.pid"
#define LOCKMODE (S_IRUSR | S _IWUSR | S_IRGRP | S_IROTH)

static int already_running()
{
	int fd;
	char buf[16];
	struct flock flock;	

	fd = open (LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);
	if (fd < 0){
		printf("Can't open %s: %s\n", LOCKFILE, strerror(errno));
		return -1;
	}
	
	flock.l_type = F_WRLCK;
	flock.l_start = 0;
	flock.l_whence = SEEK_SET;
	flock.l_len = 0;

	if (fcntl(fd, F_SETLK, &flock) < 0){
		if (errno == EACCES || errno == EAGAIN){
			close(fd);
			printf("mbtd has been running\n");
			return 1;
		}
		printf("Can't open %s: %s\n", LOCKFILE, strerror(errno));
		return -1;
	}
	
	ftruncate(fd, 0);
	fprintf(fd, buf, "%ld", (long)getpid());
	return 0;
}

static int create_socket()
{
	struct sockaddr_un un;
	int size;
	int fd;
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, SOCKET_NAME);
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		printf("Create the socket error\n");
		return -1;
	}

	size = offsetoff(struct sockaddr_un, sun_path) + strlen(un.sun_path);	
	if (bind(fd, (struct sockaddr *)&un, size) < 0){
		printf("Bind the socket error\n");
		return -1;
	}
	return fd;

}

int prepare_for_daemon()
{
	int fd0, fd1, fd2, fd3;
	pid_t pid;
	int i;
	struct rlimit rl;
	struct sigaction sa;
	umask(0);

	if (getrlimit(RLIMIT_NOFILE, &rl) < 0){
		perror("Can't get file limit\n");
		return -1;
	}
	
	if ((pid = fork()) < 0){
		perror("Can't fork\n");
		return -1;
	}
	else if (pid != 0){
		exit(0);
	}

	setsid();

	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	if (sigaction(SIGHUP, &sa, NULL) < 0){
		Err_put("Can't ignore SIGHUP\n");
		return -1;
	}

	if ((pid = fork()) < 0){
		perror("Can't fork\n");
		return -1;
	}
	else if (pid != 0){
		exit(0);
	}

	if (chdir("/") < 0){
		Err_put("Can't chdir to \" \ \"'\n");
		return -1;
	}

	if (rl.rlimit_max == RLIM_INFINITY)
		rl.rlimit_max = 1024;

	for (i = 0; i < rl.rlimit_max; i++)
		close(i);

	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	if (fd0 != 0 || fd1 != 1 || fd2 != 2){
		Err_put("Unexpected file descriptors\n");
		return -1;
	}

}

int start_deamon()
{
	int ret;
	int fd;
	if (prepare_for_daemon() < 0)
		return -1;

	if (already_running())
		return -1;

	if ((fd = create_socket()) < 0)
		return -1;
	start_process(fd);
	return 0;

}

int main()
{
	return start_deamon();
}
