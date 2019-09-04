#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
This job will generate a random integer every second for the total amount of time determined in the command line arguments and will calculate
the sum of all the integers at the end of the allotted time
*/

int main(int argc, char *argv[]){
	int executionTime = atoi(argv[1]);
	FILE *fp;

	fp = fopen("Job2_output.txt", "w");
	fprintf(fp, "Job 2 is starting up\n");
	
	int random, sum;
	for(int i = 0; i < executionTime; i++){
		random = rand();
		fprintf(fp, "Random integer %d is %d\n", i, random);
		sum += random;
		sleep(1);
	}

	fprintf(fp, "The total sum of all random integers is %d\n", sum);
	
	fprintf(fp, "Job 2 is closing\n");

	fclose(fp);

	return 0;
}
