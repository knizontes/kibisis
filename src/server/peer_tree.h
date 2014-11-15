#include "stdhead.h"
#include "peer_info.h"
//#include "const.h"
//#include "file_info.h"

#define MAXPEERS 10000
#define LEFT 0
#define RIGHT 1

/*
 * The struct peer_node models the information which the AVL peer tree needs
 * to mantain his status consistent:
 * 
 * 		> peer		holds peer informations;
 * 		>left_son	the left son node of the present node;
 * 		>right_son	the right son node of the present node;
 * 		>father		the father node of the present node;
 * 		>left_h		the height of the left tree that starts from the present node;
 * 		>right_h	the height of the right tree that starts from the present node;
 * 		>position	the position in peer_tree array of the pointer of the present node memory location;
 */

struct peer_node{
	
	struct peer_info peer;
	struct peer_node * left_son;
	struct peer_node * right_son;
	struct peer_node * father;
	int left_h;
	int right_h;
	int position;
	
};

int add_peer_node (struct peer_info * peer);
int addr_verify(char * add1,char * add2);
void print_peer_tree(struct peer_node * node,int level);
int check_peer_balance_factor(struct peer_node * node);
int balance_peer_node(struct peer_node * node,int flag);
struct peer_node * peer_predecessor(struct peer_node* node);
struct peer_node * rightest_gson_peer(struct peer_node * node);
int set_peer_father_h(struct peer_node * node);
int max_peer_h(struct peer_node * node);
struct peer_node * peer_search(char * addr, struct peer_node * node);
//int add_file_to_peer(char * addr, char * name_file);

struct peer_node * peer_tree[MAXPEERS];
struct peer_node * root_peer;
char * toremove_peers[MAXPEERS];
int torem_peers_cur;
//int cursor;
