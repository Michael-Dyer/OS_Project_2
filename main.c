#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>

#define BUF_SIZE 100


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
		printf("cant open file\n");
	}


	


	//close file and free string memory
	fclose(of);
	free(output_file);
	return 0;

}//end of main
