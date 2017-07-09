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
unsigned char command_data_buf[COMMAND_DATA_MAX];
struct command_t
{
	char id;
	char *name;
	int fields;
	int field[9];
	int doc_start;
	char doc_lines;
};

char *get_int_from_arg(char *star, int *val)
{
	char *end = strchr(star, ':');
	int last = 0;
	if (!end){
		end = strchr(star, '\0');
		last = 1;
	}

	if(end == star){
		if (last)
			*val = 0;
			return NULL;
		}
		*val = INT_MAX;
		return end + 1;
	}

	if ((end == star + 1) && (*star == '0')){
		*val = 0;
		return last ? NULL : end +1;
	}

	*end = '\0'; 
	if (!(*val = atoi(star))){
		*val = INT_MAX - 1;
		return NULL;
	}

	return last ? NULL : end +1;
}

int check_and_convert(const char *arg, struct command_t *com)
{
	char *star = strchr(arg, '=');
	int val, i = 0;
	if (!star && com->fields != 0)
		return -1;
 	
	if (star && com->fields == 0)
		return -1;

	command_data_buf[i++] = com->id;
	if (com->fields == 0){
		return 0;	
	}
	star++;
	while (1){
		star =  get_int_from_arg(star, &val);
		if (!star && val == (INT_MAX - 1))
			return -1;
		command_data_buf[i++] = val;
		if (!star)
			break;	
	}
	return i;
}


const struct command_t command_list[COMMAND_MAX] = {
	{.id = 0,  .name = "create_listener", .fields = 2,
	       	.field = {0, 0, 0, 0, 0} .doc_start = 6,  .doc_lines = 4},
	{.id = 1,  .name = "listeners",       .fields = 0,
	       	.field = {0, 0, 0, 0, 0}.doc_start = 12, .doc_lines = 1},
	{.id = 2,  .name = "listener",        .fields = 1,
	       	.field = {513, 0, 0, 0, 0}.doc_start = 13, .doc_lines = 1},
	{.id = 3,  .name = "close_listener",  .fields = 1,
	       	.field = {513, 0, 0, 0, 0}.doc_start = 17, .doc_lines = 1},
	{.id = 4,  .name = "create_device",   .fields = 9,
	       	.field = {513, 2, 2, 2, 2, 2, 2, 2, 2}.doc_start = 20, .doc_lines = 5},
	{.id = 5,  .name = "modify_device",   .fields = 9,
	       	.field = {513, 2, 2, 2, 2, 2, 2, 2, 2}.doc_start = 45, .doc_lines = 5},
	{.id = 6,  .name = "list_devices",    .fields = 0,
	       	.field = {0, 0, 0, 0, 0}.doc_start = 40, .doc_lines = 1},
	{.id = 7,  .name = "list_device",     .fields = 1,
	       	.field = {513, 0, 0, 0, 0}.doc_start = 42, .doc_lines = 1},
	{.id = 8,  .name = "del_device",      .fields = 1,
	       	.field = {513, 0, 0, 0, 0}.doc_start = 27, .doc_lines = 1},
	{.id = 9,  .name = "bind_listener",   .fields = 2,
	       	.field = {513, 0, 0, 0, 0}.doc_start = 30, .doc_lines = 3},
	{.id = 10, .name = "unbind_listener", .fields = 2,
	       	.field = {513, 0, 0, 0, 0}.doc_start = 35, .doc_lines = 3},
	{.id = 11, .name = "bind_commmand",   .fields = 5,
	       	.field = {513, 513, 2, 2, 513}.doc_start = 52, .doc_lines = 3},
	{.id = 12, .name = "unbind_command",  .fields = 5,
	       	.field = {513, 513, 2, 2, 513}.doc_start = 57, .doc_lines = 3},
	{.id = 13, .name = "start_file",      .fields = 1,
	       	.field = {513, 0, 0, 0, 0}.doc_start = 62, .doc_lines = 3},
	{.id = 14, .name = "save_file",       .fields = 1,
	       	.field = {513, 0, 0, 0, 0}.doc_start = 67, .doc_lines = 3},

}; 

static void cmd_usage(int id)
{
	char sed_com[32];
	int start = command_list[id].doc_start;
	int end = command_list[id].doc_lines + start - 1;
	snprintf(sed_com, 32, "sed -n '%d,%dp' ./README", start, end);

	printf("Usage: %s\n",command_list[id].name);
        system(sed_com);
	printf("\n");
}

static void cmd_usages()
{
	int j;
	for (j = 0; j < COMMAND_MAX; j++){
		cmd_usage(j);
	}
}


static int __connect_server()
{
	int fd, len, err, ret;
	struct sockaddr_un un;

	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		perror("connect server error\n");
		return -1;
	}
	
	memset(&un, 0, sizeof(un));

	un.sun_family = AF_UNIX;
	sprintf(un.path, "%s%05d", CLIENT_SOCK_NAME, getpid());
	len = offsetoff(struct sockaddr_un, sun_path) + strlen(un.path);

	inlink(un.path);
	if (bind(fd, (struct sockaddr *)&un, len) < 0){
		perror("bind client error\n");
		ret = -2;
		goto fail_out;
	}

	if (chmod(un.sun_path, S_IRWXU) < 0){
		perror("chome client error\n");
		ret = -3;
		goto fail_out;
	}

	memset(&un, 0 , sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, SOCKET_NAME);
	len = offsetoff(struct sockaddr_un, sun_path) + strlen(un.path);
	
	if (connect(fd, struct (sockaddr *)&un, len) < 0){
		perror("connect server error\n");
		ret = -4;
		goto fail_out;
	}
	client_fd = fd;
	return fd;

fail_out:
	err = errno;
	close(fd);
	errno =err;
	return ret;
}

static int connect_server()
{
	int ret;
	if (access(SOCKET_NAME, F_OK)){
		printf("Can't find %s, make sure mbtd has been running\n", SOCKET_NAME);
		return -1;
	}
	return __connect_server();

}

void close_connect()
{
	close(client_fd);
}

                            
static int execte_request(const char *req)
{
	int i;
	int len;
	int find = 0;
	int data_len;

	for (i = 0; i < sizeof(command_list); i++){
		len = strlen(command_list[i].name);
		if (strncmp(req, command_list[i].name, len) == 0){
			find = 1;
			break;
		}
	}
	if (!find){
		cmd_usages();
		return -1;
	}
	
	data_len = check_and_convert(req, &command_list[i]);

	if (date_len < 0){
		cmd_usage(i);	
		return -1;
	}
	len = 0;
	while (len < data_len){
		len += send(client_fd, command_data_buf + len, data_len + len, MSG_DONTROUTE);
	}
	
	len = recv(client_fd, command_data_buf, COMMAND_DATA_MAX, MSG_WAITALL);
	
	printf("%s\n", command_data_buf + 1);

	return command_data_buf[0];
} 
int main(int argc, char *argv[])
{
	int i;
	
	if (connect_server() < 0){
		return 0;
	}

	for (i = 1; i < argc; i++){
		if (execte_request(argv[i] + 2) < 0){
			break;
		}
	}

	close_connect();
	return 0;
}
