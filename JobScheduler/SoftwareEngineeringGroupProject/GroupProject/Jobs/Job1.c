#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
This job will take the execution time taken from the command line and will sprint a sleeping statement for every second it sleeps
*/

int main(int argc, char *argv[]){

	int executionTime = atoi(argv[1]);
	int priority = atoi(argv[2]);
	FILE *fp;

	fp = fopen("Job1_output.txt", "w");
	fprintf(fp, "Job 1 is starting up\n");
	
	for(int i = 0; i < executionTime; i++){
		fprintf(fp, "Sleeping for 1 second\n");
		sleep(1);
	}
	
	fprintf(fp, "Job1 is closing.");

	fclose(fp);
}
