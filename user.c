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

	key_t shm_key;
	int shm_id;
	struct Sh_mem *sh_mem_ptr;	

	shm_key = ftok(".",'0');
	shm_id = shmget(shm_key, sizeof(struct Sh_mem), IPC_CREAT | 0666);
	if (shm_id < 0) {
		perror("shmget error in user.c (child process");	
		exit(1);
	}

	sh_mem_ptr = (struct Sh_mem *) shmat(shm_id, NULL, 0);
	if (sh_mem_ptr == NULL){
		perror("shmat error in user.c");
		exit(1);
	}
	
	//shared mem is working above
	//
	//	
	//


	int child_num = atoi(argv[1]);
	int num_target = atoi(argv[2]); 
	long start_nano_secs = sh_mem_ptr->nano_secs;
	long end_time = start_nano_secs + 1000000;	

	int flag = 0;
	int i;


	for (i = 2; i <= num_target/2; ++i){
		if (num_target % i == 0) {
			flag = 1;
			break;
		}
		if (sh_mem_ptr->nano_secs >= end_time) {
			printf("this took too long\n");	
			exit(1);
		}	
	}


	long end_nano_secs = sh_mem_ptr->nano_secs;

	if( num_target == 1){
		printf("1 isn't a prime\n");
	}
	else {
		if (flag == 0){
			printf("%d is a prime number\n", num_target);
		}
		else{
			printf("%d is not prime\n", num_target);
		}
	}



	return 0;

}//end of main
