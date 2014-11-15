//#include "const.h" 


#define ADDRLEN 30
#define MAXOWNEDFILES 200


struct peer_info{
	char addr[ADDRLEN];
	int port;
//	int last_serial;
	char * owned_files[MAXOWNEDFILES];
	int files_num;
	int last_pong;
};

