/* UDP_echo.c
 * 
 * Copyright (C) 2004 Simone Piccardi
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/****************************************************************
 *
 * Program UDP_echo.c
 * Simple UDP client for echo service (port 7)
 *
 * Author: Simone Piccardi
 * May 2004
 *
 * Usage: echo -h give all info's
 *
 ****************************************************************/
/* 
 * Include needed headers
 */
#include <sys/types.h>   /* primitive system data types */
#include <unistd.h>      /* unix standard library */
#include <arpa/inet.h>   /* IP addresses conversion utilities */
#include <sys/socket.h>  /* socket constants, types and functions */
#include <stdio.h>	 /* standard I/O library */
#include <errno.h>	 /* error definitions and routines */
#include <string.h>	 /* C strings library */
#include <stdlib.h>      /* C standard library */
#include <pthread.h>
#include "client.h"
#include "y.tab.h"

//#include "macros.h"



/* Program begin */
int main(int argc, char *argv[])
{
/* 
 * Variables definition  
 */
    int i;
    pthread_t listen_t;
    
	alen=sizeof(dst_addr);

    /*
     * Input section: decode parameters passed in the calling 
     * Use getopt function
     */
    opterr = 0;	 /* don't want writing to stderr */
    while ( (i = getopt(argc, argv, "h")) != -1) {
	switch (i) {
	/* 
	 * Handling options 
	 */ 
	case 'h':  
	    printf("Wrong -h option use\n");
	    usage();
	    return(1);
	    break;
	case '?':   /* unrecognized options */
	    printf("Unrecognized options -%c\n",optopt);
	    usage();
	default:    /* should not reached */
	    usage();
	}
    }
    /* ***********************************************************
     * 
     *		 Options processing completed
     *
     *		      Main code beginning
     * 
     * ***********************************************************/
    /* create socket */
    if ( (sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	perror("Socket creation error");
	return 1;
    }
    /* initialize address */
    memset((void *) &dst_addr, 0, sizeof(dst_addr)); /* clear address */
    dst_addr.sin_family = AF_INET;                   /* address type is INET */
    dst_addr.sin_port = htons(3333);                    /* echo port is 7 */
    /* build address using inet_pton */
    if ( (inet_pton(AF_INET, argv[optind], &dst_addr.sin_addr)) <= 0) {
	perror("Address creation error");
	return 1;
    }
    //connect(sock, (struct sockaddr *) &dst_addr, sizeof(dst_addr));
    pthread_create(&listen_t,NULL,listen_thread,NULL);
    /* do read/write operations */
    Client();
    /* normal exit */
    return 0;
}
/*
 * routine to print usage info and exit 
 */
void usage(void) {
    printf("Take daytime from a remote host \n");
    printf("Usage:\n");
    printf("  daytime [-h] [-v] [host in dotted decimal form] \n");
//    printf("  -v	   set verbosity on\n");
    printf("  -r	   require reset on closing\n");
    printf("  -h	   print this help\n");
    exit(1);
}

void * listen_thread(){
	printf("listen thread started...\n");
	int nread,nwrite;
	//struct sockaddr_in src_addr;
//	memset((void *) &src_addr, 0, sizeof(src_addr)); /* clear address */
   //	src_addr.sin_family = AF_INET;                   /* address type is INET */
     //	src_addr.sin_port = htons(3333);                    /* echo port is 7 */
//	connect(rd_sock, (struct sockaddr *) &src_addr, sizeof(src_addr));
//	if (bind(rd_sock, (struct sockaddr *)&src_add, sizeof(src_add)) < 0) {
//		perror("bind error");
//		exit(1);
//	}	
	char recvbuff[MAXLINE+1];
	char pong [10]="";
	int png;	
	while(1){
		nread = recvfrom(sock,recvbuff,MAXLINE,0,NULL,NULL ); 
		if (nread < 0) {  /* error condition, stop client */
		    printf("Errore in lettura: %s\n", strerror(errno));
		    return;
		}
		
		recvbuff[nread] = '\0';   /* else read is ok, write on stdout */
		printf("--->%s\n",recvbuff);
		if (strncmp(recvbuff,"ping",4)==0){
			png=atoi(&recvbuff[5]);
			sprintf(pong,"pong %d",png);
			printf("ping %d received...\nsending %s...\n",png,pong);
			nwrite=sendto(sock,pong,strlen(pong),0,(struct sockaddr *)&dst_addr,(socklen_t ) alen);
			if (nwrite==6) printf("pong sended successfully.\n");
			else printf("an error occourred sending pong...\n");
		}		
		
	}

}

void Client() 
{
    char sendbuff[MAXLINE+1],recvbuff[MAXLINE+1];
    int nwrite,nread; 
	
    /* initialize file descriptor set */
 /*   while (1) {
	if (fgets(sendbuff, MAXLINE, stdin) == NULL) {
	    return;                /* if no input just return */
	/*} else {                   /* else we have to write to socket */
	  /*  sendbuff[strlen(sendbuff)-1]='\0';
	    printf("invio il comando «%s»...\n\n", sendbuff);
	    nwrite = sendto(sock,sendbuff,MAXLINE,0,(struct sockaddr *)&dst_addr,(socklen_t ) alen);
	    if (nwrite < 0) {      /* on error stop */
		/*printf("Errore in scrittura: %s", strerror(errno));
		return;
	    }
	    
	}
	//printf("aspetto la risposta dal server...\n");

	
    }*/
	yyparse();
}
