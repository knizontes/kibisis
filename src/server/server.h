#include <syslog.h>
#include <signal.h> 
#include <pthread.h>
#include "stdhead.h"
#include "peer_tree.h"
//#include "const.h"
//#include "file_info.h"
//#include "peer_info.h"

#define MAXLINE 256
#define BACKLOG 20

//#define MAXCONN 20		/*Il massimo numero di connessioni contemporaneamente attive sul server*/





 
//char address[300];
int sock_fd;




/* Subroutines declaration */
void usage(void);
void PrintErr(char * error);
ssize_t FullWrite(int fd, const void *buf, size_t count);
void start_server(/*int sockfd/*, struct sockaddr_in * src_addr, socklen_t *addrlen */);
int parse_comm_syntax(char * buff);
int parse_comm(char * buff);

int push_peer(char * add);
int read_add(char *dest_buff,char *src_buff);
int parse_arg(char * dest_buff, char * src_buff, int arg_num);
int add_file(char * filename, char * md5, char * owner_add);
int is_there_file(char * filename, int * index_list, int list_size);
int is_there_addr(char * address);
int sendlist(int sockfd, struct sockaddr * dest_addr, socklen_t addr_len);
int ack_check(char * buff);


/* thread declaration*/
void * peer_man();
void * file_man();
pthread_mutex_t comm_mut;
pthread_cond_t comm_cond;
pthread_mutex_t fileop_mut;
pthread_cond_t fileop_cond;

//pid_t child_make(int);
//void child_main(int);
//void lock_init();
//void lock_whait();
//void lock_release();





