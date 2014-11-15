#include "stdhead.h"
#include "file_info.h"
#include "server.h"
//#include "const.h"

#define LEFT 0
#define RIGHT 1

#define MAXFILENUM 10000000


struct file_node{
	struct file_info file;
	struct file_node * left_son;
	struct file_node * right_son;
	struct file_node * father;
	int left_h;
	int right_h;
	int position;
};


int add_file_node(struct file_info * file,char * owner);
void print_file_tree(struct file_node * node,int level);

int balance_file_node(struct file_node * node,int flag);
struct file_node * file_search(char * name, struct file_node * node);
struct file_node * file_predecessor(struct file_node* node);
struct file_node * rightest_file_gson(struct file_node * node);
int delete_file_node(struct file_node * node);
int remove_file(char * name);
int set_file_father_h(struct file_node* node);
int max_file_h(struct file_node * node);
int send_file_list(struct sockaddr_in * addr);
int send_file_list_rec(struct sockaddr_in * addr, struct file_node * node);

struct file_node * file_tree[MAXFILENUM];
struct file_node * root_file;
int filenum;
