#include "stdhead.h"
#include "server.h"
#include "file_info.h"
//#include "peer_info.h"
#include "command.h"
//#include "const.h"
//#include "peer_tree.h"
/*
Caratteristiche del server:
    1. Il server risponde a comandi del tipo <comando valori> su datarammi udp
    2. comandi con controllo a tre vie, tranne liste che richiedono un end of list
    3. struttura a tre thread:
        3a. thread principale, ascolta le richieste e le marca sull'array del thread per il mantenimento della tabella
        3b. thread secondario per il mantenimento della tabella degli utenti, consuma i comandi ricevuti e scritti sulla comm_lst
            secondo disciplina fifo inoltrando quelli destinati all'aggiornamento della tabella dei files dopo averne verificato l'indirizzo di origine.
        3c. thread secondario per il mantenimento della tabella, consuma i comandi scritti sulla file_comm_lst secondo disciplina fifo.
   4. il thread b si occupa anche di verificare tramite ping l'attività effettiva dei peer che mantiene in lista. Se il ping fallisce al quinto tentativo il peer è rimosso
   
   miglioramenti possibili:
       > prevenzione syn flood
       > piggybacking dell'avanzamento dei downloads
       > tabelle con alberi bilanciati
       > ping carico di informazioni tipo:
            - ci sono nuovi files
            - utenti in rete
            - files in rete

*/

#define MAXCOMMANDS 100000
#define NAMELEN 70


#define MAXFILEOP 10000
#define LISTOP 2
#define UPDATEOP 3
#define LEAVEOP 4
#define WHOHASOP 5

struct fileop{
	char command [COMMLEN];
	char address [20];
	short port;
	short op;
};




char * join_s = "join";
char * list_s= "list";
char * update_s = "update";
char * leave_s = "leave";
char * whohas_s= "whohas";
char * ack_s="ACK";
char * ping_s="ping";
char * pong_s="pong";


int id =0;
int ping_index;

struct command * comm_lst[MAXCOMMANDS];
int comm_wr_cur;
int comm_num;

struct fileop * fileop_lst [MAXFILEOP];
int fileop_wr_cur;
int fileop_num;

int parse_comm_syntax(char * buff);
int send_ack(char * buff, struct sockaddr_in * addr);
int addr_cmp(char * add1, char * add2);

int add_command(char * comm, struct sockaddr_in * src_addr);

void ping_f();

void * peer_man();
void * ping_man();

int joinf(struct command * comm);
int listf(struct command * comm);
int updatef(struct command * comm);
int leavef(struct command * comm);
int whohasf(struct command * comm);





struct sockaddr_in serv_add;

int main(int argc, char *argv[]){
	
	
	pthread_t peer_man_th;
	pthread_t file_man_th;
	pthread_t ping_man_th;
	    
	printf("Socket creation:");
    if ( (sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {		/*UDP!!*/
    	perror("Socket creation error");
	   	exit(1);
	}
	printf("ok...\n");
	        
	/* initialize address */
	memset((void *)&serv_add, 0, sizeof(serv_add)); /* clear server address */
	serv_add.sin_family = AF_INET;                  /* address type is INET */
	serv_add.sin_port = htons(3333);                   /*port is 3333 */
	serv_add.sin_addr.s_addr = htonl(INADDR_ANY);   /* connect from anywhere */
	        
    /* bind socket */
    printf("Bind:");
    if (bind(sock_fd, (struct sockaddr *)&serv_add, sizeof(serv_add)) < 0) {
    	perror("bind error");
	    exit(1);
	}
	printf("ok...\n");
	        
	/* release privileges and go daemon */
	printf("leaving SU privileges:");
	if (setgid(65534) !=0) { /* first give away group privileges */
	  	perror("cannot give away group privileges:");
	   	exit(1);
	}
	if (setuid(65534) !=0) { /* and only after user ... */
	 	perror("cannot give away user privileges");
	   	exit(1);
	}
	printf("success...\n\nthreads initialization:\n");
	        
	pthread_mutex_init(&comm_mut,NULL);
	pthread_cond_init(&comm_cond, NULL);
	
	pthread_mutex_init(&fileop_mut,NULL);
	pthread_cond_init(&fileop_cond,NULL);
	        
	comm_num=0;
	comm_wr_cur=0;
	
	fileop_num=0;
	fileop_wr_cur=0;
	        
    
	if (pthread_create(&file_man_th,NULL,file_man,NULL)!=0){																	/*verificare i per valori di errore*/
	  	perror("can't start file manager thread.");
	 	exit(1);
	}
	
	if (pthread_create(&peer_man_th,NULL,peer_man,NULL)!=0){
	   	printf("can't start peer manager thread.");
	   	exit(1);
	}
	
	if (pthread_create(&ping_man_th,NULL,ping_man,NULL)!=0){
		printf("can't start ping manager thread.");
		exit(1);
	}

	printf("\nstarting server:\n");
	start_server();
	            

	/* normal exit, never reached */
	return 0;

	
}

void start_server(/*int sockfd/*, struct sockaddr_in * src_addr, socklen_t *addrlen */){
	int nread,nwrite,i,n,src_add_len;
	char rd_buffer[MAXLINE+1],wr_buffer[MAXLINE+1],address[50]="", arg_addr[50]="";
	struct sockaddr_in src_addr;
	
	printf("server started...\n");
	strcpy(rd_buffer,"");
	strcpy(wr_buffer,"");
	src_add_len =sizeof(src_addr);
	
	
	
	memset(&src_addr,0x0,src_add_len);
	
	while (1){	
		nread = recvfrom(sock_fd, rd_buffer, MAXLINE,0,(struct sockaddr *)&src_addr,(socklen_t *) &src_add_len);
		rd_buffer[nread]='\0';

		
		if (!add_command (rd_buffer,&src_addr)){
			printf("Command list overflow...");
			continue;
		}
		
		
//		if (comm_num>=MAXCOMMANDS)
//			continue;
//		pthread_mutex_lock(&comm_mut);
//		comm_lst[comm_wr_cursor]=malloc(sizeof(struct command));
//		strcpy(comm_lst[comm_wr_cursor]->command,rd_buffer);
//		strcpy(comm_lst[comm_wr_cursor]->src_addr,address);
//		comm_wr_cursor = (comm_wr_cursor + 1)%MAXCOMMANDS;
//		++comm_num;
		pthread_cond_signal(&comm_cond);
//		pthread_mutex_unlock(&comm_mut);

		

	}
}

void * peer_man(){
	printf ("peer manager thread running...\n");
	struct command * present_comm;
	char address[50]="", arg[50]="";
	int i,present_comm_cur;
	
	for (i=0;i<MAXPEERS;++i){
		toremove_peers[i]=NULL;
	}
	
	while(1){
		if (comm_num<=0){
			printf("Command list empty...\n");
			pthread_cond_wait(&comm_cond,&comm_mut);
		}
		printf("______________________________________________________________________________\n");
		if ((comm_wr_cur-comm_num)<0) 
			present_comm_cur=MAXCOMMANDS-(comm_num-comm_wr_cur);
		else
			present_comm_cur=comm_wr_cur-comm_num;
		
		present_comm=comm_lst[present_comm_cur];
		
		if (strncmp(present_comm->command,ping_s,4)==0){
			ping_f();
//			printf("executing ping_f...\n");
			free(comm_lst[present_comm_cur]);
			comm_lst[present_comm_cur]=NULL;
			--comm_num;
			continue;
		}
		
		
		if (strncmp(present_comm->command,pong_s,4)==0){
			char pong_index[2]="";
			parse_arg(pong_index,present_comm->command,1);
//			printf("L'INDICE DEL PONG E' %s",pong_index);
			pong_f(&(present_comm->src),pong_index);
//			printf("executing ping_f...\n");
			free(comm_lst[present_comm_cur]);
			comm_lst[present_comm_cur]=NULL;
			--comm_num;
			continue;
		}
				
		
		inet_ntop(AF_INET, &(present_comm->src.sin_addr), address, sizeof(address));
		printf("Received «%s» command from host:%s port:%u...\n",present_comm->command,address,ntohs(present_comm->src.sin_port));
		if (check_comm(present_comm->command)<0){
			printf("Unknown command...\n");
			free(comm_lst[present_comm_cur]);
			comm_lst[present_comm_cur]=NULL;
			--comm_num;
			continue;
		}
		parse_arg(arg,present_comm->command,1);
		if (!addr_cmp(arg,address)){
			printf("No matching between the source address and the argument one...\n");
			free(comm_lst[present_comm_cur]);
			comm_lst[present_comm_cur]=NULL;
			--comm_num;
			continue;
		}
		parse_arg(arg,present_comm->command,0);

		if (strcmp(arg,join_s)!=0 && peer_search(address,root_peer)==NULL ){
			printf("The address «%s» is not a registered peer.",address);
			free(comm_lst[present_comm_cur]);
			comm_lst[present_comm_cur]=NULL;
			--comm_num;
			continue;
		}
		i=0;
		while (!send_ack(present_comm->command,&(present_comm->src))&&i<1000){
			printf("Error sending ack...\n");
			++i;
		}
		if (i>=1000){
			printf("Can't send ack.\n");
			free(comm_lst[present_comm_cur]);
			comm_lst[present_comm_cur]=NULL;
			--comm_num;
			continue;
		}
		
		if (exec_comm(present_comm)<0){
			printf("An error occourred executing «%s» command from host%s port:%u...\n",present_comm->command,address,ntohs(present_comm->src.sin_port));
			free(comm_lst[present_comm_cur]);
			comm_lst[present_comm_cur]=NULL;
			--comm_num;
			continue;
		}
		free(comm_lst[present_comm_cur]);
		comm_lst[present_comm_cur]=NULL;
		--comm_num;
		
	}
}

void * file_man(){
	printf("file manager thread running...\n");
	struct fileop * present_fileop;
	int present_fileop_cur;
	
	
	while (1){
		if (fileop_num<=0){
			printf("File operation list empty...\n");
			pthread_cond_wait(&fileop_cond,&fileop_mut);
		}
		
		
		if ((fileop_wr_cur-fileop_num)<0) 
			present_fileop_cur=MAXFILEOP-(fileop_num-fileop_wr_cur);
		else
			present_fileop_cur=fileop_wr_cur-fileop_num;
				
		present_fileop=fileop_lst[present_fileop_cur];
		if (present_fileop->op==LISTOP)
			listop(present_fileop);
		else if (present_fileop->op==UPDATEOP)
			updateop(present_fileop);
		else if (present_fileop->op==LEAVEOP)
			leaveop(present_fileop);
		else if (present_fileop->op==WHOHASOP)
			whohasop(present_fileop);
		free(fileop_lst[present_fileop_cur]);
		fileop_lst[present_fileop_cur]=NULL;
		--fileop_num;		
	}
}

void * ping_man(){
	printf("ping manager thread running...\n");
	ping_index=0;
	
	while(1){
		sleep (15);
		printf("adding ping command...\n");
		if (!add_command (ping_s,NULL)){
			printf("Command list overflow...");
			continue;
		}
		pthread_cond_signal(&comm_cond);
	}
	
}

int exec_comm(struct command * comm){
	char comm_type[20]="";
	int n;
	parse_arg(comm_type,comm->command,0);
	
	
	if (strcmp(comm_type, join_s)==0)
		n=joinf(comm);								/*if the command is a join command the return code is 1*/
	else if (strncmp(comm_type, list_s,4)==0)
		n=listf(comm);
	else if (strncmp(comm_type,update_s,6)==0)
		n=updatef(comm);
	else if (strncmp(comm_type,leave_s,5)==0)
		n=leavef(comm);							/*leave command code is 4*/
	else if (strncmp(comm_type,whohas_s,6)==0)
		n=whohasf(comm);
	/*else if (strnmcp(comm_type,ping_s,4)==0)
		ping_f();*/
	else n=-1;
	return n;
		
	
	
}

int check_comm(char * buff,char * src_add){
	
//	char address[50];
	
	if (strncmp(buff,join_s,4)==0 ||strncmp(buff, list_s,4)==0||strncmp(buff,update_s,6)==0||strncmp(buff,leave_s,5)==0||strncmp(buff,whohas_s,6)==0||strcmp(buff,ping_s)==0)
		return 1;
	
	return (-1);
	
}

/*
 * add_command() returns 0 if the command array is completely filled. 
 */

int add_command(char * comm, struct sockaddr_in * src_addr){
	int b=1,i=0;
	if (comm_num>=MAXCOMMANDS) return 0;
	comm_lst[comm_wr_cur]=malloc(sizeof(struct command));
	strcpy(comm_lst[comm_wr_cur]->command,comm);
	if (strcmp(comm,ping_s)==0){
		printf("ping command added...\n");
		++comm_num;
		comm_wr_cur=(comm_wr_cur+1)%MAXCOMMANDS;
		return 1;
	}
		
//	strcpy(comm_lst[comm_wr_cur]->src_addr,add);
//	comm_lst[comm_wr_cur]->src_port=port;
	memset((void *)&(comm_lst[comm_wr_cur]->src), 0, sizeof(src_addr));
	comm_lst[comm_wr_cur]->src.sin_addr=src_addr->sin_addr;
	comm_lst[comm_wr_cur]->src.sin_port=src_addr->sin_port;
	comm_lst[comm_wr_cur]->src.sin_family=AF_INET;
	
	
	/*Increasing the number of commands present in the list comm_num and
	 * upgrading the writing cursor in the array comm_wr_cur.*/
	++comm_num;
	comm_wr_cur=(comm_wr_cur+1)%MAXCOMMANDS;
	return 1;
}

/*
 * add_fileop() adds a new fileoperation in the fileoperation array.
 * It returns 0 if the array is already completely filled, and 1 if the new insertion 
 * has succes.
 */

int add_fileop(char * comm, char * addr, short port, short op){
	if (fileop_num>=MAXFILEOP){
		printf("fileop array filled...\n");
		return 0;
	}
	fileop_lst[fileop_wr_cur]=malloc(sizeof(struct fileop));
	strcpy(fileop_lst[fileop_wr_cur]->command,comm);
	strcpy(fileop_lst[fileop_wr_cur]->address,addr);
	fileop_lst[fileop_wr_cur]->port=port;
	fileop_lst[fileop_wr_cur]->op=op;
	++fileop_num;
	fileop_wr_cur= (fileop_wr_cur + 1)%MAXFILEOP;
	
	return 1;
}

/*
 * parse_arg() takes the arg_num argument from src_buff and copy it in dest_buff.
 * Note: the argumants are «arg0,arg1,...».
 */

int parse_arg(char * dest_buff, char * src_buff, int arg_num){
	int i,j,n=0;
	for (i=0;i<strlen(src_buff);++i){
		if (n==arg_num) break;
		if (src_buff[i]==' ') ++n;
	}
	if (n<arg_num)
		return -1;
	n=1;
	j=0;
	while(n){
		if (src_buff[i]!=' ' && i<strlen(src_buff) ){
			dest_buff[j]=src_buff[i];
			++i;
			++j;
		}
		else {
			dest_buff[j]='\0';
			n=0;
		}
	}
	return 1;
}

/*
 * get_argnum returns 0 if the argument string is an empty string,
 * else it returns the argument number in the string
 * Notes: the argument are counted as
 * 			<arg1> <arg2> ... <argn>     -> return value: n.
 */

int get_argnum(char * comm){
	int i,argnum;
	if (strcmp(comm,"")!=0)
		argnum=1;
	else return 0;
	for (i=0; i<strlen(comm);++i){
		if (comm[i]==' ')
			++argnum;
	}
	printf("the command has %d arguments\n",argnum);
	return argnum;
}

/*
 * addr_cmp() returns 1 if the two arguments are equals, 0 otherwise.
 */
int addr_cmp(char * add1, char * add2){
	if (strcmp(add1,add2)==0) return 1;
	return 0;
}

/*
 * send_ack () returns 0 if it can't write all the ack message into the socket,
 * 1 otherwise.
 */

int send_ack(char * buff, struct sockaddr_in * addr){
	int nwrite;
	char wr_buffer[100]="";
	strcpy(wr_buffer,"ACK ");
	strcat(wr_buffer,buff);
	printf("Sending %s...\n",wr_buffer);
	nwrite=sendto(sock_fd, wr_buffer, strlen(wr_buffer),0,(struct sockaddr *) addr,(socklen_t ) sizeof(*addr));
	if (nwrite!=((int)strlen(wr_buffer)))
		return 0;
	return 1;
}

int joinf(struct command * comm){
	
	int i;
	if (get_argnum(comm->command)!=2){
		printf("wrong argument number...\n");
		return -1;
	}
	struct peer_info apeer;
	parse_arg(apeer.addr,comm->command,1);
//	apeer.last_serial=0;
	apeer.port=ntohs(comm->src.sin_port);
	
	if (add_peer_node(&apeer)<0){
		printf("Error in function joinf: can't add correctly the peer %s...",apeer.addr);
		return -1;
	}
	printf("Peer «%s» correctly added...\n",apeer.addr);
	return 1;
}

int listf(struct command * comm){
	if (get_argnum(comm->command)!=2){
		printf("wrong argument number...\n");
		return -1;
	}
	char address[20];
	struct peer_node * peer;
	inet_ntop(AF_INET, &(comm->src.sin_addr), address, sizeof(address));
	peer=peer_search(address,root_peer);
	if (peer==NULL)
			return -1;							/*If the address does not exist listf returns -1*/
//	peer->peer.port=(comm->src.sin_port);
	if(!add_fileop(comm->command,address,ntohs(comm->src.sin_port),LISTOP)){
		return -2;
	}
//	printf("Signaling operation fo file thread...\n");
	pthread_cond_signal(&fileop_cond);
//	printf("list operation signaled...\n");
	return 2;
}

/*
 * Notes: 	The update command format is: 
 * 					update <src_address> <namefile> <md5sum>
 */

int updatef(struct command * comm){
	
	if (get_argnum(comm->command)<5){
		printf("wrong argument number...\n");
		return -1;
	}
	
	char filename[70]="";
	char address[20]="";
	struct peer_node * peer;
	printf("%s\n",comm->command);
	inet_ntop(AF_INET, &(comm->src.sin_addr), address, sizeof(address));
	peer=peer_search(address,root_peer);
		if (peer==NULL)
		return -1;							/*If the address does not exist updatef returns -1*/
//	peer->peer.port=(comm->src.sin_port);
	parse_arg(filename,comm->command,2);
	if (update_peer(address,filename)<0) return -2;    /*If the update of the peer is unsuccessful updatef returns -2 */
	if(!add_fileop(comm->command,address,ntohs(comm->src.sin_port),UPDATEOP)){
		return -2;
	}
	pthread_cond_signal(&fileop_cond);
	return 3;
}

int leavef(struct command * comm){
	int i,removed;
	struct peer_node * prnd;
	if (get_argnum(comm->command)!=2){
		printf("wrong argument number...\n");
		return -1;
	}
	char addr [20]="";
	parse_arg(addr,comm->command,1);
	prnd = peer_search(addr,root_peer);
	/*
	 * setting the operation over the file_tree.
	 */
	removed=0;
	for (i=0;(i<MAXOWNEDFILES)&&(removed<prnd->peer.files_num);++i){
		if (prnd->peer.owned_files[i]!=NULL){
			if(!add_fileop(prnd->peer.owned_files[i],addr,ntohs(comm->src.sin_port),LEAVEOP))
				return -2;
			++removed;
			pthread_cond_signal(&fileop_cond);
		}
	}
	if (remove_peer(addr)){
		printf ("Peer «%s» correctly removed...\n",addr);
//		if(!add_fileop(comm->command,addr,ntohs(comm->src.sin_port),LEAVEOP)){
//			return -2;
//		}
		
		return 4;
	}
	printf ("Peer «%s» was not in the net...\n",addr);
	
	return -1;
}

int whohasf(struct command * comm){
	if (get_argnum(comm->command)<3){
		printf("wrong argument number...\n");
		return -1;
	}
	char address[20]="";
	struct peer_node * peer;
	inet_ntop(AF_INET, &(comm->src.sin_addr), address, sizeof(address));
	peer=peer_search(address,root_peer);
	if (peer==NULL)
		return -1;							/*If the address does not exist whohasf returns -1*/
//	peer->peer.port=(comm->src.sin_port);
	if(!add_fileop(comm->command,address,ntohs(comm->src.sin_port),WHOHASOP)){
		return -2;
	}
	pthread_cond_signal(&fileop_cond);
	return 5;
}

void ping_f(){
	int n;
	if (root_peer==NULL)
		return;
	n=send_ping(root_peer,ping_index,sock_fd);
	if (n!=1)
		printf("Something wrong in pingf?\n");
	ping_index=(ping_index+1)%5;
	torem_peers_cur=0;
	n=find_unreached_peers(root_peer,ping_index);
	remove_unreached_peers();
}

int pong_f(struct sockaddr_in * src, char * pong_index){
	char address[20]="";
	inet_ntop(AF_INET, &(src->sin_addr), address, sizeof(address));
//	printf("Received «%s» command from host:%s port:%u...\n",present_comm->command,address,ntohs(present_comm->src.sin_port));
	printf("received pong %s from peer %s\n",pong_index,address);
	return peer_pong(address,pong_index);
}

int listop(struct fileop * fop){
	struct sockaddr_in dest;
	int nwrite,n;
//	char tosend[20]="Pacchetto!!!";
	
	
	dest.sin_family = AF_INET;
	dest.sin_port=htons(fop->port);
	inet_pton(AF_INET,fop->address,&(dest.sin_addr.s_addr));
	
	if (n=send_file_list(&dest)<0){
		printf("An error occourred in sending file list...\n");
		return -1;
	}
	else if (n==0){
		printf("End Of List\n");
	}
	
	return 1;
	
	
}

int updateop(struct fileop * fop){
	struct file_info * file;
	char dim[20]="";
	file=malloc (sizeof(struct file_info));
	if (file==NULL)
		return -1;
	
	parse_arg(file->name,fop->command,2);
	parse_arg(file->md5sum,fop->command,3);
	parse_arg(dim,fop->command,4);
	file->mb_dim=atoi(dim);
//	printf("UPDATEOP\n");
	add_file_node(file,fop->address);
//	printf("UPDATEOP\n");
	return 1;
}

int leaveop(struct fileop * fop){
	int n;
	printf("ch1\n");
	if(n=remove_owner(fop->command,fop->address)<0){
		printf("ERROR:Something wrong appened removing the owner %s from file %s",fop->address,fop->command);
		return -1;
	}
	printf("ch2\n");
	if (n=0){
		printf("Can't remove the owner %s from the file %s:file not found.\n",fop->address,fop->command);
		return 0;
	}
	return 1;
}

int whohasop(struct fileop * fop){
	struct sockaddr_in dest;
	int nwrite,n;
	char filename[NAMELEN];
//	char tosend[20]="Pacchetto!!!";
		
		
	dest.sin_family = AF_INET;
	dest.sin_port=htons(fop->port);
	inet_pton(AF_INET,fop->address,&(dest.sin_addr.s_addr));
	parse_arg(filename,fop->command,2);
	
	
	if (n=send_own_list(&dest,filename)<0){
		printf("An error occourred in sending file list...\n");
		return -1;
	}
	else if (n==0){
		printf("End Of List\n");
	}
		
	return 1;
		
		
	return 1;
}

int remove_unreached_peers(){
	int i;
	struct command comm;
	strcpy(comm.command,"leave ");
	for (i=0;i<torem_peers_cur;++i){
		printf("?????????????????????%s\n",toremove_peers[i]);
		strcpy(comm.command,"leave ");
		strcat(comm.command,toremove_peers[i]);
		leavef(&comm);
		free(toremove_peers[i]);
		toremove_peers[i]=NULL;
	}
}





















