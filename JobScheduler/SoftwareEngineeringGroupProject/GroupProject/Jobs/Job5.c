#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[]){

	int executionTime = atoi(argv[1]);
	
	FILE *fp;

	fp = fopen("Job5_output.txt", "w");
	
	fprintf(fp, "Job 5 is starting up\n");

	for(int i = 0; i < executionTime; i++){
		int power = (int) pow(i, i + 1);
		fprintf(fp, "%d squared is %d\n", i, power);
		sleep(1);
	}
	fprintf(fp, "Job 5 is closing\n");

	fclose(fp);

	return 0;
}
