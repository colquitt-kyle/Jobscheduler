#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
This job will print lines of asterisks depending on the size of the execution time. Every second it will print one line
and each line will have one more asterisk than the last.
*/

int main(int argc, char *argv[]){
	
	int executionTime = atoi(argv[1]);
	FILE *fp;
	
	fp = fopen("Job3_output.txt", "w");
	fprintf(fp, "Job 3 is starting\n");

	for(int i = 0; i < executionTime; i++){
		for(int j = 0; j <= i; j++){
			fprintf(fp, "*");
		}
		fprintf(fp, "\n");
		sleep(1);
	}

	fprintf(fp, "Job 3 is closing\n");

	fclose(fp);
	
	
	return 0;
}
