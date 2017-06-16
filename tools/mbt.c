#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define COMMAND_MAX  15
#define REQUEST_MAX  256
#define DOCUMENT ./README  
struct command_t
{
	char id;
	char *name;
	int doc_start;
	char doc_lines;
};

const struct command_t command_list[COMMAND_MAX] = {
	{.id = 0,  .name = "create_listener", .doc_start = 6,  .doc_lines = 4},
	{.id = 1,  .name = "listeners",       .doc_start = 12, .doc_lines = 1},
	{.id = 2,  .name = "listener",        .doc_start = 13, .doc_lines = 1},
	{.id = 3,  .name = "close_listener",  .doc_start = 17, .doc_lines = 1},
	{.id = 4,  .name = "create_device",   .doc_start = 20, .doc_lines = 5},
	{.id = 5,  .name = "modify_device",   .doc_start = 45, .doc_lines = 5},
	{.id = 6,  .name = "list_devices",    .doc_start = 40, .doc_lines = 1},
	{.id = 7,  .name = "list_device",     .doc_start = 42, .doc_lines = 1},
	{.id = 8,  .name = "del_device",      .doc_start = 27, .doc_lines = 1},
	{.id = 9,  .name = "bind_listener",   .doc_start = 30, .doc_lines = 3},
	{.id = 10, .name = "unbind_listener", .doc_start = 35, .doc_lines = 3},
	{.id = 11, .name = "bind_commmand",   .doc_start = 52, .doc_lines = 3},
	{.id = 12, .name = "unbind_command",  .doc_start = 57, .doc_lines = 3},
	{.id = 13, .name = "start_file",      .doc_start = 62, .doc_lines = 3},
	{.id = 14, .name = "save_file",       .doc_start = 67, .doc_lines = 3},

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

int create_socket()

int connect_server()
{

}

void close_server()
{

}

                            
static void send_request(int id,char *arg, char *buf)
{
	int i = 0;
	int str_len = 0;
	
	*buf = (char)(id & 0xFF);
	strncpy(buf + 1, arg, REQUEST_MAX - 1);

	
	
} 
int main(int argc, char *argv[])
{
	int i, j;
	int find;
	int str_len;
	char request[REQUEST_MAX];
	for (i = 1; i < argc; i++){
		find = 0;
		for (j = 0; j < COMMAND_MAX; j++){
			str_len = strlen(command_list[j].name);
			if (strncmp(&argv[i][2], command_list[j].name, str_len) == 0){
				find = 1;
				cmd_usage(j);
				break;

			}
		}
		if (find == 0){
			argv[i][2 + str_len] = '\0';
			printf("no find the command %s\n", &argv[i][2]);
			cmd_usages();
		}			
	}
}
