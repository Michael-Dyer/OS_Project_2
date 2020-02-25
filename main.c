#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "oss.h"


int main(int argc, char *argv[]) {
	//for getopt
	int opt;

	//fork() returns 0 for children 
	//getpid() returns that processes pid
	pid_t pid;
	


	int max_children = 4;
	//conncurrent children
	int con_children = 2;
	int seq_start = 3;
	// sequence increment
	int seq_i = 1;

	//string that will be output file
	char *output_file = malloc(256);

	//self note, understanding getopt now
	//the : will be stored in var optarg
	//in the args case
	while ((opt = getopt(argc, argv, "hn:s:b:i:o:")) != -1) {
		switch (opt) {
			case 'h':
				//change this
				printf("Help message here\n"); 
				exit(EXIT_FAILURE);
			case 'n':
				max_children = atoi(optarg);	
				if (max_children > 20){
					max_children = 20;
				}
				break;
			case 's':
				con_children = atoi(optarg);
				break;
			case 'b':
				seq_start = atoi(optarg);
				break;
			case 'i':
				seq_i = atoi(optarg);
				break;
			case 'o':
				strcpy(output_file, optarg);
				strncat(output_file, ".output", 7);					
	
				break;

		}
	}

	//set out_putfile to default if it hasnt been set 	
	if (!output_file[0]){
		strcpy(output_file, "oss.output");
	}


	//makesure output_file ends in .txt for easy clean up	
	FILE *of = fopen(output_file, "w");	
	
	if(!of){
		perror("cant open file\n");
		exit(1);	
	}



	




	//set up vars to have shared meme	
	key_t shm_key;
	int shm_id;
	struct Sh_mem *sh_mem_ptr;

	//make a key and get shared memory	
	shm_key = ftok(".", '0');	
	shm_id = shmget(shm_key, sizeof(struct Sh_mem), IPC_CREAT | 0666); 
	if (shm_id < 0){
		perror("shmget error in main.c");
		exit(1);
	}

	//this will actually put the mem into shared memory	
	sh_mem_ptr = (struct Sh_mem *) shmat(shm_id, NULL, 0);
	if (sh_mem_ptr == NULL){
		perror("shmat error");
		printf("error");
		exit(1);
	}
	
	sh_mem_ptr->secs = 10080;


	
	//close file and free all memory
	shmdt((void *) sh_mem_ptr);
	
	shmctl(shm_id, IPC_RMID, NULL);

	fclose(of);
	free(output_file);


	return 0;

}//end of main
