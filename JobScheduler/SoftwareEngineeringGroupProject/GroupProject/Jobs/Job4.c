#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
This job will produce a recursive factorial for each second of the execution time 
*/

unsigned int factorial(unsigned int n) 
{ 
    if (n == 0) 
      return 1; 
    return n*factorial(n-1); 
} 

int main(int argc, char *argv[]){
	int executionTime = atoi(argv[1]);

	FILE *fp;

	fp = fopen("Job4_output.txt", "w");
	
	fprintf(fp, "Job 4 is starting up\n");

	for(int i = executionTime; i > 0; i--){
		int fac = factorial(i);
		fprintf(fp, "Factorial of %d is %d\n", i, fac);
		sleep(1);
	}

	fprintf(fp, "Job 4 is closing.");
	fclose(fp);

	return 0;
}
