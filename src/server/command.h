#include "stdhead.h"
//#include "const.h"

struct command;

#define COMMLEN 200


struct command {
	
	char command [COMMLEN];
//	char src_addr [50];
//	short src_port;
	struct sockaddr_in src;
//	char * arg;
//	int serial;
	
};

void commcpy (struct command *dest_comm,struct command *src_comm);

