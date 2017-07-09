#ifndef ____MBT____________________H____
#define ____MBT____________________H____
#define COMMAND_MAX  15
#define REQUEST_MAX  256
#define DOCUMENT ./README  
#define SOCKET_NAME "~/modbus_socket"

#define COMMAND_DATA_MAX 256
struct command_t
{
	char id;
	char *name;
	int fields;
	int field[9];
	int doc_start;
	char doc_lines;
};
#define C_LIST_ID     0
#define L_LISTS_ID    1
#define L_LIST_ID     2
#define O_LIST_ID     3

#define C_DIV_ID     4
#define M_DIV_ID     5
#define L_DIV_ID     6
#define L_DIVS_ID    7
#define D_DIV_ID     8
#define B_DIV_ID     9
#define U_DIV_ID     10

#define B_COM_ID     11
#define U_COM_ID     12
#define S_FILE_ID    13
#define C_FILE_ID    14


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
#endif
