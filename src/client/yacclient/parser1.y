%{
#include <stdio.h>

#include <sys/types.h>   /* primitive system data types */
#include <unistd.h>      /* unix standard library */
#include <arpa/inet.h>   /* IP addresses conversion utilities */
#include <sys/socket.h>  /* socket constants, types and functions */
#include <stdio.h>	 /* standard I/O library */
#include <errno.h>	 /* error definitions and routines */
#include <string.h>	 /* C strings library */
#include <stdlib.h>   
#include "client.h"
#include "y.tab.h"


void yyerror(const char *str){
	fprintf(stderr,"error: %s\n",str);
}

int yywrap(){
	return 1;
}

/*int main(){
	yyparse();
}*/
%}

%token JOINCOMM LEAVECOMM UPDATECOMM LISTCOMM WHOHASCOMM ADDRESS NAME NUMBER

%%

commands:
	| commands command
	;

command:joincomm
	| leavecomm
	| updatecomm
	| whohascomm
	| listcomm
	;

joincomm:
	JOINCOMM ADDRESS
	{
		printf("join %s command correctly inserted\n",$2);
		char sendbuff[MAXLINE+1],recvbuff[MAXLINE+1];
		printf("???\n");
    		int nwrite,nread;
		printf("1");
		strcpy(sendbuff,"join");
		printf("2\n");
		strcat(sendbuff," ");
		strcat(sendbuff,$2);
		printf("sending command «%s»...\n\n", sendbuff);
		sendto(sock,sendbuff,MAXLINE,0,(struct sockaddr *)&dst_addr,(socklen_t ) alen);/* 
		nwrite = sendto(sock,sendbuff,MAXLINE,0,(struct sockaddr *)&dst_addr,(socklen_t ) alen);
	    	if (nwrite < 0) {      /* on error stop *//*
			printf("Errore in scrittura: %s", strerror(errno));
	    	}*/
	}
	;

leavecomm:
	LEAVECOMM ADDRESS
	{	
		printf("leave %s command correctly inserted\n",$2);
		char sendbuff[MAXLINE+1],recvbuff[MAXLINE+1];
    		int nwrite,nread;
		strcpy(sendbuff,"leave");
		strcat(sendbuff," ");
		strcat(sendbuff,$2);
		printf("sending command «%s»...\n\n", sendbuff);
		sendto(sock,sendbuff,MAXLINE,0,(struct sockaddr *)&dst_addr,(socklen_t ) alen);
	}
	;

updatecomm:
	UPDATECOMM ADDRESS NAME NAME NUMBER
	{
		printf("update %s %s %s %s command correctly inserted\n",$2,$3,$4,$5);
		char sendbuff[MAXLINE+1],recvbuff[MAXLINE+1];
    		int nwrite,nread;
		strcpy(sendbuff,"update");
		strcat(sendbuff," ");
		strcat(sendbuff,$2);
		strcat(sendbuff," ");
		strcat(sendbuff,$3);
		strcat(sendbuff," ");
		strcat(sendbuff,$4);
		strcat(sendbuff," ");
		strcat(sendbuff,$5);
		printf("sending command «%s»...\n\n", sendbuff);
		sendto(sock,sendbuff,MAXLINE,0,(struct sockaddr *)&dst_addr,(socklen_t ) alen);
	}
	;

listcomm:
	LISTCOMM ADDRESS
	{
		printf("list %s command correctly inserted\n",$2);
		char sendbuff[MAXLINE+1],recvbuff[MAXLINE+1];
    		int nwrite,nread;
		strcpy(sendbuff,"list");
		strcat(sendbuff," ");
		strcat(sendbuff,$2);
		printf("sending command «%s»...\n\n", sendbuff);
		sendto(sock,sendbuff,MAXLINE,0,(struct sockaddr *)&dst_addr,(socklen_t ) alen);

	}
	;	

whohascomm:
	WHOHASCOMM ADDRESS NAME
	{
		printf("whohas command correctly inserted\n");
		char sendbuff[MAXLINE+1],recvbuff[MAXLINE+1];
    		int nwrite,nread;
		strcpy(sendbuff,"whohas");
		strcat(sendbuff," ");
		strcat(sendbuff,$2);
		strcat(sendbuff," ");
		strcat(sendbuff,$3);
		printf("sending command «%s»...\n\n", sendbuff);
		sendto(sock,sendbuff,MAXLINE,0,(struct sockaddr *)&dst_addr,(socklen_t ) alen);

	}
	;
