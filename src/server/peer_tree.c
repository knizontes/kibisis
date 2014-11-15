#include "peer_tree.h"
//#include "const.h"
#define NAMELEN 70



int add_peer_node(struct peer_info * peer){
	int cursor,i;
	int b=1,bigger;
	struct peer_node * present_node;
	cursor =0;
	
	present_node=peer_search(peer->addr,root_peer);
	if (present_node!=NULL) 
		return 1;
	
	while (b){
		if (peer_tree[cursor]==NULL)
			b=0;
		else ++cursor;
	}
	
	//Allocating memory for the new node
	peer_tree[cursor]=malloc(sizeof(struct peer_node));
	peer_tree[cursor]->position=cursor;
	strncpy(peer_tree[cursor]->peer.addr,peer->addr,ADDRLEN);
	peer_tree[cursor]->peer.port=peer->port;
	peer_tree[cursor]->peer.files_num=0;
	for (i=0;i<20;++i){
		peer_tree[cursor]->peer.owned_files[i]=NULL;
	}
	
	/*The new node will be a leaf, so both the left and the right tree which starts from it 
	will have height 0*/
	peer_tree[cursor]->left_h=0;
	peer_tree[cursor]->right_h=0;
	
	if (root_peer==NULL){
		root_peer=peer_tree[cursor];
//		present_level=0;
		return 2;													/*the function returns 2 if the tree was empty before peer_node insertion*/
	}
	
	present_node=root_peer;
	
	b=1;
//	present_level=1;
	int cancellami=0;
	
	while (b){
		++cancellami;
		bigger=addr_verify(peer->addr,present_node->peer.addr);
		if(bigger==0){
			free(peer_tree[cursor]);
			peer_tree[cursor]=NULL;
			return 0;
		}
		else if (bigger==1){
			if ((present_node->right_son)==NULL){
				present_node->right_son=peer_tree[cursor];
				peer_tree[cursor]->father=present_node;
				check_peer_balance_factor(peer_tree[cursor]);
				return 1;
			}
			else {
				present_node=present_node->right_son;
//				++present_level;
			}
		}
		else if(bigger==2){
			if ((present_node->left_son)==NULL){
				present_node->left_son=peer_tree[cursor];
				peer_tree[cursor]->father=present_node;
				check_peer_balance_factor(peer_tree[cursor]);
				return 1;
			}
			else {
				present_node=present_node->left_son;
//				++present_level;
			}
		}
		if (cancellami>1000)return -1;
	}
	
	
	
}

//int set_h(struct peer_node* node){
//	if (node->father==NULL)
//		return 1;
//	
//	int max_peer_h;
//	
//	if ((node->left_h)>(node->right_h))
//		max_peer_h=(node->left_h);
//	else max_peer_h=(node_right_h);
//	
//	if (node->father->left_son==node)
//		node->father->left_h=max_peer_h+1;
//	else node->father->right_h=max_peer_h+1;
//	
//	set_h(node->father);
//	
//}

int balance_peer_node(struct peer_node * node,int flag){
	int balancing_factor;
	struct peer_node * temp;
	printf("balancing %s node...\n",node->peer.addr);
	/*Left flag*/
	if (flag==LEFT){
		/*Left-left condition, right rotation*/
		if (max_peer_h(node->left_son)==0){
			
			printf("left-left condition...\n");
			
			temp=(node->left_son);
			
			if (node->father!=NULL){
				if((node->father->left_son)==node)
					node->father->left_son=temp;
				else node->father->right_son=temp;
			}
						
			temp->father=(node->father);
			node->father=temp;
			
			node->left_son=(temp->right_son);			
			
			if (node->left_son!=NULL)
				node->left_son->father=node;       

			temp->right_son=node;
						
			if (temp->father==NULL)
				root_peer=temp;
						
			set_peer_father_h(node);			
			set_peer_father_h(temp);
			
			
		}
		/*Left-right condition, */
		else {
			printf("left-right condition...\n");
			temp=(node->left_son->right_son);
			
			if (node->father!=NULL){
				if((node->father->left_son)==node)
					node->father->left_son=temp;
				else node->father->right_son=temp;
			}
			
			temp->father=node->father;
			node->father=temp;
			node->left_son->father=temp;
			node->left_son->right_son=(temp->left_son);
			
			if (node->left_son->right_son!=NULL)
				node->left_son->right_son->father=(node->left_son);
			
			temp->left_son=(node->left_son);
			node->left_son=(temp->right_son);
			
			if (node->left_son!=NULL)
				node->left_son->father=node;
			
			temp->right_son=node;
			
			if (temp->father==NULL)
				root_peer=temp;
			
			set_peer_father_h(node);
			set_peer_father_h(temp->left_son);
			set_peer_father_h(temp);
			
		}	
	}
	/*Right flag*/
	else{
	/*Right-right condition, left rotation*/
		printf("right-right condition...\n");
		if (max_peer_h(node->right_son)==1){
			
			temp=(node->right_son);
			
			if (node->father!=NULL){
				if((node->father->left_son)==node)
					node->father->left_son=temp;
				else node->father->right_son=temp;
			}
			
			temp->father=(node->father);
			node->father=temp;
			node->right_son=(temp->left_son);
			
			if (node->right_son!=NULL)
				node->right_son->father=node;

			temp->left_son=node;
					
			if (temp->father==NULL)
				root_peer=temp;
			
			set_peer_father_h(node);
			set_peer_father_h(temp);
					
					
		}
		else {
			/*Right-left condition, */
			printf("right-left condition...\n");
			temp=(node->right_son->left_son);
			
			if (node->father!=NULL){
				if((node->father->left_son)==node)
					node->father->left_son=temp;
				else node->father->right_son=temp;
			}
			
			temp->father=node->father;
			node->father=temp;
			node->right_son->father=temp;
			node->right_son->left_son=(temp->right_son);
			if (node->right_son->left_son!=NULL)
				node->right_son->left_son->father=(node->right_son);

			temp->right_son=(node->right_son);
			node->right_son=(temp->left_son);
			
			if (node->right_son!=NULL)
				node->right_son->father=node;
			
			temp->left_son=node;
					
			if (temp->father==NULL)
				root_peer=temp;
					
			set_peer_father_h(node);
			set_peer_father_h(temp->right_son);
			set_peer_father_h(temp);
					
		}	
	}
	return 1;
}


/*
 * peer_search returns a pointer to the node wich matches its address with arguments,
 * or NULL if no matching was found.
 * Notes: recursive.
 */
struct peer_node * peer_search(char * addr, struct peer_node * node){
	
//	struct peer_node * pres_n;
//	pres_n=root_peer;
	if (node==NULL)
		return NULL;
	int n;
	n=addr_verify(addr,node->peer.addr);
	if (n==0){
		printf("%s matched!\n",addr);
		return node;
	}
	if (n==1){
		if (node->right_son==NULL) 
			return NULL;
		return peer_search(addr,node->right_son);
	}
	if(n==2){
		if (node->left_son==NULL)
			return NULL;
		return peer_search(addr,node->left_son);
	}
	return NULL;
}


/*
 * remove_peer() remove the peer with the same address as argument if any and returns 1,
 * if no nodes with argument address is matched returns 0;
 */
int remove_peer(char * addr){
	struct peer_node * node;
	node = peer_search(addr,root_peer);
	if (node==NULL) 
		return 0;
	return (delete_node(node));
}

/*
 * delete_node() removes a node from the tree.
 * Notes: after the rimotion the balance of tree is still effective.
 */
int delete_node(struct peer_node * node){
	
	struct peer_node * pred;
	
	if ((node->left_h==0)){
		if(node->right_h==0){
			printf("check1\n");
			if (node->father!=NULL){
				if (node->father->left_son==node){
					node->father->left_son=NULL;
					node->father->left_h=0;
				}
				else {
					node->father->right_son=NULL;
					node->father->right_h=0;
				}
				check_peer_balance_factor(node->father);
			}
			else root_peer=NULL;
			printf("check2\n");
			peer_tree[node->position]=NULL;
			free(node);
			return 1;
		}
		
		if (node->father==NULL){
			printf("check4\n");
			root_peer=node->right_son;
			node->right_son->father=NULL;
			printf("check5\n");
			peer_tree[node->position]=NULL;
			free(node);
			return 1;
		}
		printf("check6\n");
		node->right_son->father=(node->father);
		
		if (node->father!=NULL){
			if (node->father->left_son==node)
				node->father->left_son=(node->right_son);
			else node->father->right_son=(node->right_son);
			printf("check7\n");
			check_peer_balance_factor(node->right_son);
			printf("check8\n");
		}
		
		printf("check9\n");
		
		peer_tree[node->position]=NULL;
		free(node);
		printf("check10\n");
		return 1;
		
	}
	if (node->right_son==NULL){
		if (node->father==NULL){
			printf("check11\n");
			root_peer=node->left_son;
			node->left_son->father=NULL;
			peer_tree[node->position]=NULL;
			free(node);
		
			return 1;
		}
		
		printf("check12\n");
		node->left_son->father=(node->father);
		
		if (node->father!=NULL){
			if (node->father->left_son==node)
				node->father->left_son=(node->left_son);
			else node->father->right_son=(node->left_son);
			check_peer_balance_factor(node->left_son);
		}
		
		printf("check13\n");
		
		peer_tree[node->position]=NULL;
		free(node);
		
		return 1;
	}
	
	printf("check14\n");
	pred= peer_predecessor(node);
	strcpy(node->peer.addr,pred->peer.addr);
//	node->peer.last_serial=(pred->peer.last_serial);
	node->peer.port=(pred->peer.port);
	
	printf("check15\n");
	
	if (pred!=node->left_son)
		pred->father->right_son=pred->left_son;
	else node->left_son=(pred->left_son);
	
	if (pred->left_son!=NULL){	
		pred->left_son->father=pred->father;
		check_peer_balance_factor(pred->left_son);
	}
	else{
		pred->father->right_h=0;
		check_peer_balance_factor(pred->father);
	}
	printf("check16\n");
	
	printf("check16-b\n");
	peer_tree[pred->position]=NULL;
	printf("check17\n");
	free(pred);
	printf("check18\n");
	return 1;
	
}


/*
 * pred () returns a pointer to the peer_predecessor node of the argument.
 */
struct peer_node* peer_predecessor(struct peer_node* node){
	return (rightest_gson_peer(node->left_son));
}

/*
 * rightest_gson_peer() returns the bigger grandson node of the argument.
 * Notes: recursive.
 */
struct peer_node * rightest_gson_peer(struct peer_node * node){
	if (node->right_son==NULL) return node;
	return (rightest_gson_peer(node->right_son));
}

int max_peer_h(struct peer_node * node){
	if ((node->left_h)>(node->right_h))
		return 0;
	return 1;
}

int set_peer_father_h(struct peer_node* node){
	
	int l_max_peer_h,r_max_peer_h;
	
	if (node->left_son==NULL){
		node->left_h=0;
		if (node->right_son==NULL){
			node->right_h=0;
			return 0;
		}
		else {
			if ((node->right_son->left_h)>(node->right_son->right_h))
					r_max_peer_h=(node->right_son->left_h);
				
			else r_max_peer_h=(node->right_son->right_h);
			node->right_h=r_max_peer_h+1;
			return r_max_peer_h;
		}
	}
	
	if (node->right_son==NULL){
		if ((node->left_son->left_h)>(node->left_son->right_h))
				l_max_peer_h=(node->left_son->left_h);
			
			else l_max_peer_h=(node->left_son->right_h);
		node->right_h=0;
		node->left_h=l_max_peer_h;
		return l_max_peer_h;
	}
	
	if ((node->left_son->left_h)>(node->left_son->right_h))
		l_max_peer_h=(node->left_son->left_h);
	
	else l_max_peer_h=(node->left_son->right_h);

	if ((node->right_son->left_h)>(node->right_son->right_h))
		r_max_peer_h=(node->right_son->left_h);
	
	else r_max_peer_h=(node->right_son->right_h);
	
	node->left_h=l_max_peer_h+1;
	node->right_h=r_max_peer_h+1;
	
	if (l_max_peer_h>r_max_peer_h) return l_max_peer_h;	
	
	return r_max_peer_h;

	
}

void print_peer_tree(struct peer_node * node,int level){
	if (node==NULL) {
		printf("---THE PEER TREE IS EMPTY---\n\n");
		return;
	}
	printf("%d->%s, left_h:%d, right_h:%d, l_son:%s, r_son:%s.\n",level,node->peer.addr,node->left_h,node->right_h,node->left_son->peer.addr,node->right_son->peer.addr);
	if ((node->right_son)==NULL && (node->left_son)==NULL) return;
	if ((node->right_son)!=NULL)
		print_peer_tree(node->right_son,level+1);
	if ((node->left_son)!=NULL)
		print_peer_tree(node->left_son,level+1);
	
}


/*
 * addr_verify compares add1 and add2 and returns 0 if the two address are equals,
 * 1 if add1 is biggerr than add2, 2 if add2 is bigger than add1.
 */

int addr_verify(char * add1,char * add2) {
	
	int a1[4],a2[4],i,n,c,j;
	char add_num[4]="";
	
	n=0;
	c=0;
//	printf("%d\n",(int)strlen(add1));
	for (i=0;i<strlen(add1);++i){
		if (add1[i]=='.') {
//			printf("->%s\n",add_num);
			a1[n]=atoi(add_num);
//			printf("%d\n",a1[n]);
			++n;
			c=i+1;
			for (j=0;j<4;++j)
				add_num[j]='\0';
			continue;
		}
		add_num[i-c]=add1[i];
			
	}
	a1[n]=atoi(add_num);
//	for (i=0;i<4;++i)
//		printf("%d\n",a1[i]);
	
	for (j=0;j<4;++j)
		add_num[j]='\0';
	
	c=0;
	n=0;
	
	for (i=0;i<strlen(add2);++i){
		if (add2[i]=='.') {
//			printf("->%s\n",add_num);
			a2[n]=atoi(add_num);
	//			printf("%d\n",a1[n]);
			++n;
			c=i+1;
			for (j=0;j<4;++j)
				add_num[j]='\0';
			continue;
		}
		add_num[i-c]=add2[i];
				
	}
	a2[n]=atoi(add_num);
//	for (i=0;i<4;++i)
//		printf("%d\n",a2[i]);
	
	for (i=0; i<4; ++i){
		if (a1[i]>a2[i]) return 1;
		else if (a1[i]<a2[i]) return 2;
		else if (a1[i]==a2[i]) continue;
		else return -1;
	}
	return 0;
	
}

/*
 * The check_peer_balance_factor function update the tree left or right height
 * of the father of node, then call itself over the father of node.
 * 
 * If while checking the difference between left tree height left_h and the right one right_h
 * has module >= 2, then balance() is called over node. 
 * 
 * 
 */

int check_peer_balance_factor(struct peer_node * node){

	int max_peer_h,bal_f;
	
	if((node->left_h)>(node->right_h)){
		max_peer_h=(node->left_h);
		bal_f = (node->left_h)-(node->right_h);
		if(bal_f>=2){
			balance_peer_node(node,LEFT);
			return 2;							/*the function returns 2 if a balancemend occourred*/
		}
	}
	
	else{
		max_peer_h=(node->right_h);
		bal_f = (node->right_h)-(node->left_h);
		if(bal_f>=2){
			balance_peer_node(node,RIGHT);
			return 2;							/*the function returns 2 if a balancemend occourred*/
		}
	}	
	
	if (root_peer == node){
		return 1;
	}
	if ((node->father->left_son)==node)
		(node->father->left_h)=max_peer_h+1;
	else if ((node->father->right_son)==node)
		(node->father->right_h)=max_peer_h+1;
	else{
		printf("An error occourred in AVL tree structure. Exiting...\n");
		return -1;
	}
	
	return check_peer_balance_factor(node->father);
	
}
//
//int update_peer(char * apeer,char * file){
//	int i,removed;
//	struct peer_node * pn;
//	pn = search_peer(apeer,root_peer);
//	removed=0;
//	for (i=0;i<MAXOWNEDFILES && removed<pn->peer.files_num;++i){
//		if (pn->peer.owned_files[i]!=NULL){
//			if (remove_owner(pn->peer.owned_files[i],pn->peer.addr))
//				printf("Removed owner %s from file %s",pn->peer.addr,pn->peer.owned_files[i]);
//			++removed;
//		}
//	}
//}

/*
 * update_peer() returns -1 if no file matches with the argument one;
 * returns -2 if the peer owns the maximum number of files already;
 * returns 1 if the update is successful.
 */

int update_peer(char * peer, char * file ){
	struct peer_node * pn;
	pn=peer_search(peer,root_peer);
	if (pn==NULL)
		return -1;
	if (pn->peer.files_num>=MAXOWNEDFILES)
		return -2;
	int i;
	for (i=0;i<MAXOWNEDFILES;++i){
		if (pn->peer.owned_files[i]==NULL)
			break;
	}
	pn->peer.owned_files[i]=calloc(NAMELEN,sizeof(char));
	strcpy(pn->peer.owned_files[i],file);
	++(pn->peer.files_num);
	return 1;
}

int send_ping(struct peer_node * peer,int index,int socket_fd){
	int nwrite,nl,nr;
	char ping[10]="";
	struct sockaddr_in ping_dest;
	//ping_dest=malloc(sizeof(struct sockaddr_in));
	memset(&ping_dest,0,sizeof(struct sockaddr_in));
	ping_dest.sin_family = AF_INET;
	inet_pton(AF_INET,peer->peer.addr,&(ping_dest.sin_addr.s_addr));
	ping_dest.sin_port=htons(peer->peer.port);
	sprintf(ping,"ping %d",index);
	printf("Sending ping to host:%s port:%d\n", peer->peer.addr,peer->peer.port);
	nwrite=sendto(socket_fd, ping, strlen(ping),0,(struct sockaddr *) &ping_dest,(socklen_t ) sizeof(ping_dest));
	if (peer->left_son!=NULL)
		nl=send_ping(peer->left_son,index,socket_fd);
	else nl=1;
	if (peer->right_son!=NULL)
		nr=send_ping(peer->right_son,index,socket_fd);
	else nr=1;
	
	return (nl&&nr);
	
}
int find_unreached_peers(struct peer_node * node,int index){
	int nl,nr,d;
	if (node->left_son!=NULL)
		nl=remove_unreached_peers(node->left_son,index);
	if (node->right_son!=NULL)
		nr=remove_unreached_peers(node->right_son,index);
	if (node->peer.last_pong==index){
		printf("removed the node %s: it seems to be unreachable...\n",node->peer.addr);
		toremove_peers[torem_peers_cur]=malloc(sizeof(node->peer.addr));
		strcpy (toremove_peers[torem_peers_cur],node->peer.addr);
		++torem_peers_cur;
	}
		
	return 1;
}



int peer_pong(char * address, char * index){
	int pong_index=atoi(index);
	struct peer_node * node;
	node = peer_search (address,root_peer);
	if (node==NULL)
		return -1;
	node->peer.last_pong=pong_index;
	return 1;
	
}















