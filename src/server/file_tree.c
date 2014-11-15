#include "file_tree.h"
//#include "const.h"





/*
 * name_compare() compares name1 and name2 and returns 0 if name1 matches name2,
 * 1 if name1 is greater then name2, 2 if name2 is greater than name1.
 * 
 */
int name_compare(char * name1, char * name2){
	int n;
	n=strcoll(name1,name2);
	if (n>0)
		return 1;
	else if (n<0)
		return 2;
	else if (n==0) 
		return 0;
	return -1;
}



int add_file_node(struct file_info * file,char * owner){
	
	struct file_node * flnd;
	int i;
	flnd=file_search(file->name,root_file);
	if (flnd!=NULL){
		for (i=0;i<OWNERS;++i){
			if (flnd->file.owners[i]==NULL)
				break;
		}
		if (i==OWNERS){
			if(flnd->file.owners[flnd->file.own_cur]==NULL)
				flnd->file.owners[flnd->file.own_cur]=calloc(ADDRLEN,sizeof(char));
			strcpy(flnd->file.owners[flnd->file.own_cur],owner);
			flnd->file.own_cur=(flnd->file.own_cur+1)%OWNERS;
			if (flnd->file.own_num<20)
				++(flnd->file.own_num);
			return 3;
		}
		flnd->file.owners[i]=calloc(ADDRLEN,sizeof(char));
		strcpy(flnd->file.owners[i],owner);
		if (flnd->file.own_num<20)
			++(flnd->file.own_num);
		return 3;
	}
	
	if (filenum>=MAXFILENUM){
		printf("Reached maximum file number...\n");
		return -2;
	}
	
	printf("ALASKA\n");
	
	
	int cursor;
	int b=1,bigger;
	struct file_node * present_node;
	cursor =0;
	
	while (b){
		if (file_tree[cursor]==NULL)
			b=0;
		else ++cursor;
	}
	++filenum;
	
	//Allocating memory for the new node
	file_tree[cursor]=malloc(sizeof(struct file_node));
	file_tree[cursor]->position=cursor;
	strncpy(file_tree[cursor]->file.name,file->name,NAMELEN);
	strncpy(file_tree[cursor]->file.md5sum,file->md5sum,MD5LEN);
	file_tree[cursor]->file.mb_dim=(file->mb_dim);
	
	printf("BARBEQUE\n");
	
	
	for (i=0;i<20;++i)
		file_tree[cursor]->file.owners[i]=NULL;
	file_tree[cursor]->file.owners[0]=calloc(ADDRLEN,sizeof(char));
	strcpy(file_tree[cursor]->file.owners[0],owner);
	file_tree[cursor]->file.own_cur=1;
	file_tree[cursor]->file.own_num=1;
	
	printf("CANADA\n");
	/*The new node will be a leaf, so both the left and the right tree which starts from it 
	will have height 0*/
	file_tree[cursor]->left_h=0;
	file_tree[cursor]->right_h=0;
	
	if (root_file==NULL){
		root_file=file_tree[cursor];
		return 2;													/*the function returns 2 if the tree was empty before file_node insertion*/
	}
	printf("DALLAS\n");
	present_node=root_file;
	
	b=1;
//	present_level=1;
//	int cancellami=0;
	
	while (b){
		bigger=name_compare(file->name,present_node->file.name);
		if(bigger==0){
			free(file_tree[cursor]);
			file_tree[cursor]=NULL;
			--filenum;
			return 0;
		}
		else if (bigger==1){
			if ((present_node->right_son)==NULL){
				present_node->right_son=file_tree[cursor];
				file_tree[cursor]->father=present_node;
				check_balance_factor(file_tree[cursor]);
				return 1;
			}
			else {
				present_node=present_node->right_son;
//				++present_level;
			}
		}
		else if(bigger==2){
			if ((present_node->left_son)==NULL){
				present_node->left_son=file_tree[cursor];
				file_tree[cursor]->father=present_node;
				check_balance_factor(file_tree[cursor]);
				return 1;
			}
			else {
				present_node=present_node->left_son;
			}
		}
	}
	
	
	
}

/*
 * The check_balance_factor function update the tree left or right height
 * of the father of node, then call itself over the father of node.
 * 
 * If while checking the difference between left tree height left_h and the right one right_h
 * has module >= 2, then balance() is called over node. 
 * 
 * 
 */

int check_balance_factor(struct file_node * node){

	int max_file_h,bal_f;
	
	if((node->left_h)>(node->right_h)){
		max_file_h=(node->left_h);
		bal_f = (node->left_h)-(node->right_h);
		if(bal_f>=2){
			balance_file_node(node,LEFT);
			return 2;							/*the function returns 2 if a balancemend occourred*/
		}
	}
	
	else{
		max_file_h=(node->right_h);
		bal_f = (node->right_h)-(node->left_h);
		if(bal_f>=2){
			balance_file_node(node,RIGHT);
			return 2;							/*the function returns 2 if a balancemend occourred*/
		}
	}	
	
	if (root_file == node){
		return 1;
	}
	if ((node->father->left_son)==node)
		(node->father->left_h)=max_file_h+1;
	else if ((node->father->right_son)==node)
		(node->father->right_h)=max_file_h+1;
	else{
		printf("An error occourred in AVL tree structure. Exiting...\n");
		return -1;
	}
	
	return check_balance_factor(node->father);
	
}



void print_file_tree(struct file_node * node,int level){
	int i;
	if (node==NULL) {
		printf("---THE FILE TREE IS EMPTY---\n\n");
		return;
	}
	if (node->left_son==NULL){
		if (node->right_son==NULL){
			printf("%d->%s, left_h:%d, right_h:%d, l_son:%s, r_son:%s.\n",level,node->file.name,node->left_h,node->right_h,"NULL","NULL");
			printf("Owners %d:",node->file.own_num);
			for (i=0;i<OWNERS;++i){
				if (node->file.owners[i]!=NULL)
					printf("%s ",node->file.owners[i]);
			}
			printf("\n\n");
			return;
		}
		else {
			printf("%d->%s, left_h:%d, right_h:%d, l_son:%s, r_son:%s.\n",level,node->file.name,node->left_h,node->right_h,"NULL",node->right_son->file.name);
			printf("Owners %d:",node->file.own_num);
			for (i=0;i<OWNERS;++i){
				if (node->file.owners[i]!=NULL)
					printf("%s ",node->file.owners[i]);
			}
			printf("\n\n");
			print_file_tree(node->right_son,level+1);
		}
	}
	else {
		if (node->right_son==NULL){
			printf("%d->%s, left_h:%d, right_h:%d, l_son:%s, r_son:%s.\n",level,node->file.name,node->left_h,node->right_h,node->left_son->file.name,"NULL");
			printf("Owners %d:",node->file.own_num);
			for (i=0;i<OWNERS;++i){
				if (node->file.owners[i]!=NULL)
					printf("%s ",node->file.owners[i]);
			}
			printf("\n\n");
			print_file_tree(node->left_son,level+1);
		}
		else {
			printf("%d->%s, left_h:%d, right_h:%d, l_son:%s, r_son:%s.\n",level,node->file.name,node->left_h,node->right_h,node->left_son->file.name,node->right_son->file.name);
			printf("Owners %d:",node->file.own_num);
			for (i=0;i<OWNERS;++i){
				if (node->file.owners[i]!=NULL)
					printf("%s ",node->file.owners[i]);
			}
			printf("\n\n");
			print_file_tree(node->left_son,level+1);
			print_file_tree(node->right_son,level+1);
		}
	}

	
}


int balance_file_node(struct file_node * node,int flag){
	int balancing_factor;
	struct file_node * temp;
	printf("balancing %s node...\n",node->file.name);
	/*Left flag*/
	if (flag==LEFT){
		/*Left-left condition, right rotation*/
		if (max_file_h(node->left_son)==0){
			
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
				root_file=temp;
						
			set_file_father_h(node);			
			set_file_father_h(temp);
			
			
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
				root_file=temp;
			
			set_file_father_h(node);
			set_file_father_h(temp->left_son);
			set_file_father_h(temp);
			
		}	
	}
	/*Right flag*/
	else{
	/*Right-right condition, left rotation*/
		printf("right-right condition...\n");
		if (max_file_h(node->right_son)==1){
			
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
				root_file=temp;
			
			set_file_father_h(node);
			set_file_father_h(temp);
					
					
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
				root_file=temp;
					
			set_file_father_h(node);
			set_file_father_h(temp->right_son);
			set_file_father_h(temp);
					
		}	
	}
	return 1;
}

int max_file_h(struct file_node * node){
	if ((node->left_h)>(node->right_h))
		return 0;
	return 1;
}


int set_file_father_h(struct file_node* node){
	
	int l_max_file_h,r_max_file_h;
	
	if (node->left_son==NULL){
		node->left_h=0;
		if (node->right_son==NULL){
			node->right_h=0;
			return 0;
		}
		else {
			if ((node->right_son->left_h)>(node->right_son->right_h))
					r_max_file_h=(node->right_son->left_h);
				
			else r_max_file_h=(node->right_son->right_h);
			node->right_h=r_max_file_h+1;
			return r_max_file_h;
		}
	}
	
	if (node->right_son==NULL){
		if ((node->left_son->left_h)>(node->left_son->right_h))
				l_max_file_h=(node->left_son->left_h);
			
			else l_max_file_h=(node->left_son->right_h);
		node->right_h=0;
		node->left_h=l_max_file_h;
		return l_max_file_h;
	}
	
	if ((node->left_son->left_h)>(node->left_son->right_h))
		l_max_file_h=(node->left_son->left_h);
	
	else l_max_file_h=(node->left_son->right_h);

	if ((node->right_son->left_h)>(node->right_son->right_h))
		r_max_file_h=(node->right_son->left_h);
	
	else r_max_file_h=(node->right_son->right_h);
	
	node->left_h=l_max_file_h+1;
	node->right_h=r_max_file_h+1;
	
	if (l_max_file_h>r_max_file_h) return l_max_file_h;	
	
	return r_max_file_h;

	
}


/*
 * file_search returns a pointer to the node wich matches its name with arguments,
 * or NULL if no matching was found.
 * Notes: recursive.
 */
struct file_node * file_search(char * name, struct file_node * node){
	
//	struct file_node * pres_n;
//	pres_n=root_file;
	if (node==NULL)
		return NULL;
	int n;
	n=name_compare(name,node->file.name);
	if (n==0){
		printf("%s matched!\n",name);
		return node;
	}
	if (n==1){
		if (node->right_son==NULL) 
			return NULL;
		return file_search(name,node->right_son);
	}
	if(n==2){
		if (node->left_son==NULL)
			return NULL;
		return file_search(name,node->left_son);
	}
	return NULL;
}


/*
 * remove_file() remove the file with the same name as argument if any and returns 1,
 * if no nodes with argument address is matched returns 0;
 */
int remove_file(char * name){
	struct file_node * node;
	node = file_search(name,root_file);
	if (node==NULL) 
		return 0;
	return (delete_file_node(node));
}

/*
 * delete_file_node() removes a node from the tree.
 * Notes: after the rimotion the balance of tree is still effective.
 */
int delete_file_node(struct file_node * node){
	
	struct file_node * pred;
	
	--filenum;
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
				check_balance_factor(node->father);
			}
			else root_file=NULL;
			printf("check2\n");
			file_tree[node->position]=NULL;
			free(node);
			return 1;
		}
		
		if (node->father==NULL){
			printf("check4\n");
			root_file=node->right_son;
			node->right_son->father=NULL;
			printf("check5\n");
			file_tree[node->position]=NULL;
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
			check_balance_factor(node->right_son);
			printf("check8\n");
		}
		
		printf("check9\n");
		
		file_tree[node->position]=NULL;
		free(node);
		printf("check10\n");
		return 1;
		
	}
	if (node->right_son==NULL){
		if (node->father==NULL){
			printf("check11\n");
			root_file=node->left_son;
			node->left_son->father=NULL;
			file_tree[node->position]=NULL;
			free(node);
		
			return 1;
		}
		
		printf("check12\n");
		node->left_son->father=(node->father);
		
		if (node->father!=NULL){
			if (node->father->left_son==node)
				node->father->left_son=(node->left_son);
			else node->father->right_son=(node->left_son);
			check_balance_factor(node->left_son);
		}
		
		printf("check13\n");
		
		file_tree[node->position]=NULL;
		free(node);
		
		return 1;
	}
	
	printf("check14\n");
	pred= file_predecessor( node);
	strcpy(node->file.name,pred->file.name);
	strcpy(node->file.md5sum,pred->file.md5sum);
//	node->file.last_serial=(pred->file.last_serial);
	
	printf("check15\n");
	
	if (pred!=node->left_son)
		pred->father->right_son=pred->left_son;
	else node->left_son=(pred->left_son);
	
	if (pred->left_son!=NULL){	
		pred->left_son->father=pred->father;
		check_balance_factor(pred->left_son);
	}
	else{
		pred->father->right_h=0;
		check_balance_factor(pred->father);
	}
	printf("check16\n");
	
	printf("check16-b\n");
	file_tree[pred->position]=NULL;
	printf("check17\n");
	free(pred);
	printf("check18\n");
	return 1;
	
}


/*
 * pred () returns a pointer to the file_predecessor node of the argument.
 */
struct file_node * file_predecessor(struct file_node* node){
	return (rightest_file_gson(node->left_son));
}

/*
 * rightest_file_gson() returns the bigger grandson node of the argument.
 * Notes: recursive.
 */
struct file_node * rightest_file_gson(struct file_node * node){
	if (node->right_son==NULL) return node;
	return (rightest_file_gson(node->right_son));
}


/*
 * send_file_list send the filelist to the argument address.
 * It returns 0 if something wrong appened writing in the socket,
 * 0 if no files are shared in the net and 1 otherwise.
 * 
 */
int send_file_list(struct sockaddr_in * addr){
	if (root_file!=NULL)
		return send_file_list_rec (addr,root_file);
	else 
		return 0;	
}

int send_file_list_rec(struct sockaddr_in * addr, struct file_node * node){
	int nwrite,left,right;
	char to_send[100],dim[20];
	strcpy(to_send,node->file.name);
	strcat(to_send," ");
	sprintf(dim,"%d\0",node->file.mb_dim);
	strcat(to_send,dim);
	printf("sending %s\n",to_send);
	nwrite=sendto(sock_fd, to_send, strlen(to_send),0,(struct sockaddr *) addr,(socklen_t ) sizeof(struct sockaddr_in));
	if (nwrite!=((int)strlen(to_send))){
		printf("??? %d-%d\n",nwrite,(int)strlen(to_send));
//		return -1;
	}
	if (node->left_son!=NULL)
		left=send_file_list_rec(addr,node->left_son);
	if (node->right_son!=NULL){
		right=send_file_list_rec(addr,node->right_son);
	}
//	printf("---%d\n",(left&right));
//	printf("---%d\n",(left&&right));
	return (left||right);
}

/*
 * remove_owner removes the owner from the owner list of the argument file.
 * If no file is found with the name as the argument the function returns 0;
 * if a file was found and the owner successfully removed returns 1;
 * if a file was found and the owner was the only owner, the file is removed and the function returns 2;
 * if something wrong appened the function returns -1.
 */

int remove_owner(char * name, char * owner){
	int i;
	struct file_node *flnd;
	flnd = file_search(name,root_file);
	if (flnd==NULL){
		printf("The peer %s has registered the file %s which is not in the file tree...\n",owner,name);
		return -1;
	}
	for (i=0;i<20;++i){
		if (flnd->file.owners[i]!=NULL){
//			printf("%s-%s",owner,(flnd->file.owners[i]));
			if (strcmp(owner,(flnd->file.owners[i]))==0){
//				printf("^^^^^^^^CH%d\n",i);
				free(flnd->file.owners[i]);
				flnd->file.owners[i]=NULL;
				--(flnd->file.own_num);
				if (flnd->file.own_num<1){
					if (remove_file(name))
						return 2;
					else return -1;
				}
				return 1;
				
			}
		}
	}
	printf("The owner %s was not registered as owner of the file %s.\n",owner,name);
	return -1;
}

/*
 * send_own_list() returns 0 if the argument filename does not match with any file in  the file tree,
 * otherwise returns 1 and send to addr the list of file owners.
 */

int send_own_list(struct sockaddr_in * addr, char * filename){
	struct file_node * flnd;
	int i,sended,nwrite;
	flnd = file_search(filename,root_file);
	if (flnd==NULL){
		printf("No filename %s has been found in the net...\n",filename);
		return 0;
	}
	printf("SeNdInG LiSt-%d...\n",flnd->file.own_num);
	sended=0;
	for (i=0;(i<OWNERS) && (sended < flnd->file.own_num);++i){
		if (flnd->file.owners[i]!=NULL){
			printf("sending %s...\n",flnd->file.owners[i]);
			/*nwrite=*/sendto(sock_fd, flnd->file.owners[i], strlen(flnd->file.owners[i]),0,(struct sockaddr *) addr,(socklen_t ) sizeof(struct sockaddr_in));
			++sended;
		}
	}
	return 1;
}









