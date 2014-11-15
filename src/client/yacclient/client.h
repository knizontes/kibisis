#define MAXLINE 256
void usage(void);
void Client();
void SigTERM_hand(int sig);
void * listen_thread();

int sock;
struct sockaddr_in dst_addr;
int alen;
