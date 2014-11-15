#include "stdhead.h"
#include "peer_info.h"

int main(){
	
	int i,j,cur;
	struct peer_info fl1,fl2,fl3;
	char name[30];
	strcpy(fl1.addr,"peer1");
	strcpy(fl2.addr,"peer2");
	strcpy(fl3.addr,"peer3");
	
	printf("1\n");
	for (i=0;i<MAXOWNEDFILES;++i){
		fl1.owned_files[i]=NULL;
		fl2.owned_files[i]=NULL;
		fl3.owned_files[i]=NULL;
		fl1.files_num=0;
		fl2.files_num=0;
		fl3.files_num=0;
	}
	printf("2\n");
	for (i=0;i<4;++i){
		if (fl1.files_num>MAXOWNEDFILES)
			break;
		
		printf("aggiungi un file a peer1:");
		for (cur=0;cur<MAXOWNEDFILES;++cur){
			if (fl1.owned_files[cur]==NULL)
				break;
		}
		fl1.owned_files[cur]=calloc(30,sizeof(char));
		scanf("%s",fl1.owned_files[cur]);
		++fl1.files_num;
	}
	for (i=0;i<fl1.files_num;++i){
		
		printf("%s\n",fl1.owned_files[i]);
	}
	return 0;
	
	
	
	
}