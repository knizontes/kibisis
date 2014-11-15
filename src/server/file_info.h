//#include "const.h"


#define NAMELEN 70
#define MD5LEN 33
#define ADDRLEN 30
#define OWNERS 20


struct file_info {
	
	int id;
	char name[70];
	char md5sum[33];
	int mb_dim;
	char * owners [20];
	int own_cur;
	int own_num;
	
};


