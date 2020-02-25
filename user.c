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
	
	printf("this should display 10000: %d\n",sh_mem_ptr->secs);
	

	return 0;

}//end of main
