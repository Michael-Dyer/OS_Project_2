#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "oss.h"

char** get_string(int index,int test);
void sig_handler(int sig);

//making these global so sig_handler can use
int* pid_arr; 
int end_arr;

int main(int argc, char *argv[]) {
	//for getopt
	int opt;

	//fork() returns 0 for children 
	//getpid() returns that processes pid
	pid_t pid;
	//this will be a string array for excvp 
	char** args;
	
	int max_children = 4;
	//conncurrent children
	int con_children = 2;
	int seq_start = 3;
	// sequence increment
	int seq_i = 1;
	int status;

	//string that will be output file
	char *output_file = malloc(256);

	//this will allow us to call the signal handler
	//either ctr c or 2 minutes will call sig handler
	signal(SIGINT, sig_handler);
	signal(SIGALRM, sig_handler);
	alarm(120);	

	//self note, understanding getopt now
	//the : will be stored in var optarg
	//in the args case
	while ((opt = getopt(argc, argv, "hn:s:b:i:o:")) != -1) {
		switch (opt) {
			case 'h':
				//change this
				printf("welcome to my program!\n"); 
				printf("n(max children) default is 4\n"); 
				printf("s default is 2\n");  
				printf("b default is 3\n");  
				printf("i default is 1\n");  
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

	//make sure con_children can't be more than max children
	if (con_children > max_children) {
		con_children = max_children;
	}

	if (max_children <= 0 || con_children <= 0 || seq_start <= 0 || seq_i <= 0){
		perror("cant open file\n");
		exit(1);
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

	//now fill shared mem array with 0
	//if it doesn't get to terminate, 
	//it will stay 0	
	int i;	
	//for(i = 0; i < max_children; i++){
	//	sh_mem_ptr->prime_arr[i] = 0;
	//}	

	int nums_to_test[max_children];
	int pid_arr[max_children];
	int end_arr[max_children];

	//fill nums_to_test with values that will be tested
	for(i = 0; i < max_children; i++) {
		nums_to_test[i] = seq_start;
		seq_start = seq_start + seq_i;
	}

	int p_ran = 0;
	int p_running = 0;
	//this will execute the string
	//args = get_string(0,99);
	//execvp(args[0], args);	
	//



	while (p_ran < max_children) {
	sh_mem_ptr->nano_secs += 10000;
	
	if (p_running <= con_children) {
		
		p_running++;
		if ((pid = fork()) < 0) {
			perror("forking child process failed\n");
			exit(1);	
		}
		else if(pid ==0 && p_running <= con_children){

			char s_index[20];
			char s_test[20];	
			sprintf(s_index,"%d", p_ran);
			sprintf(s_test,"%d", nums_to_test[p_ran]);
			//this is right
			char *args[] = {"./user", s_index , s_test , NULL};

			execvp(args[0], args);	
		}
		//this will show that execvp is done
		p_ran++;	
	}//end of if loop	

	if(WIFEXITED(status)){
		end_arr[p_ran] = sh_mem_ptr->nano_secs;
		p_running--;
	}
	
	}//end of while
	
	
	//for(i = 0; i < max_children; i++){
	//	printf("number is: %d\n",sh_mem_ptr->prime_arr[i]);
	//}	
	

	//print firt line
	for(i = 0; i < max_children; i++){
		fprintf(of, "child %d ended at %d nanoseconds, ", i, end_arr[i]);	
		sh_mem_ptr->prime_arr[i] ;
	}	
		fprintf(of, "\n");

	//seconnd line 

	for(i = 0; i < max_children; i++){
		if (sh_mem_ptr->prime_arr[i] > 0) { 
			fprintf(of, "%d is prime, ", sh_mem_ptr->prime_arr[i]);	
		}
		else if (sh_mem_ptr->prime_arr[i] < 0) {
			fprintf(of, "%d is not prime, ", (0 - sh_mem_ptr->prime_arr[i]));	
		} 
	}
		fprintf(of, "\n");

	//third line
	for(i = 0; i < max_children; i++){
		if (sh_mem_ptr->prime_arr[i] = 0) { 
			fprintf(of, "%d this process wasn't able to handle the prime, sorry ", i);	
		}
	}	





	
	//close file and free all memory
	shmdt((void *) sh_mem_ptr);
	shmctl(shm_id, IPC_RMID, NULL);

	fclose(of);
	free(output_file);


	return 0;

}//end of main




void sig_handler(int sig) {


	printf("ur out\n");

	exit(0);



}

