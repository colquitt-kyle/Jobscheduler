/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.c
 * Author: kyle6
 *
 * Created on September 24, 2018, 11:55 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdint.h>
/* Error Code */
#define EINVAL       1
#define E2BIG        2

#define MAXMENUARGS  7
#define MAXCMDLINE   200

#define MAX 256

//initialize command line functions
void menu_execute(char *line, int isargs);
int cmd_run(int nargs, char **args);
int cmd_quit(int nargs, char **args);
void showmenu(const char *name, const char *x[]);
int cmd_helpmenu(int n, char **a);
int cmd_dispatch(char *cmd);
void cmd_list();
int cmd_changePolicy(int nargs, char **args);
int cmd_test(int nargs, char **args);

//cpu time calulation vairables
clock_t start1, end1;
double cpu_time_used1;
int procCompleted;
float tempT;
float testCpu;


//commands init
enum Command {
    NoCommand, FCFS, Shortest, Priority, Run, Help
};

//init enum for scheduler
enum ScheduleType {
    FirstComeFirstServe, ShortestJobFirst, PriorityOrdering
};

//init the structure for jobs
struct Job {
    char *name;
    int executionTime;
    int priority;
    char *arrivalTime[100];
    time_t timeMilliseconds;
    bool executed;
    int test;

};
//-----------------------------------Initialization of variables------------------------------------

pthread_cond_t condition1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition2 = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

struct Job jobQueue[5] = {0};

bool schedulingInProgress = false;

enum ScheduleType CurrentScheduleFunction = FirstComeFirstServe;
enum Command CurrentCommand = NoCommand;

//------------------------------------End initialization of varibles--------------------------------
//calls the list function through the cmd
void cmd_list(){
    list();
    
}

//takes in parameters from user to add job to job queue
int cmd_run(int nargs, char **args) {
   //init vairables
    char name[10];
    int cmd2 = 4;
    int execT;
    int pri;
   //wrong num of args warning
    if (nargs != 4) {
        printf("Usage: run <job> <time> <priority>\n");
        return EINVAL;
    }
    //switches the current command
    switchCommand(cmd2);
    
    //changes args to ints
    sscanf(args[2], "%d", &execT);
    sscanf(args[3], "%d", &pri);
    
 
    //calls run command to add job
    run(args[1], execT, pri);
   

    return 0; 
}

//changes the scheduling policy based on user input
int cmd_changePolicy(int nargs, char **args)
{
   
    int cmd;
  
    if(args[0][0] == 's'){
        cmd = 1;
   }
     if(args[0][0] == 'p'){
         cmd = 2;
   }
     if(args[0][0] == 'f'){
         cmd = 3;
   }
   
    
   switchCommand(cmd);
   
   return 0;

}

//performance evaluation
int cmd_test(int nargs, char **args) {
    //init variables
    int numJobs;
    int pri;
    int cpuTime;
    int maxCpuTime;
    int cmd5;

    if (nargs < 6) {
        printf("Usage: test <policy> <num_of_jobs> <priority_levels> <min_CPU_time> <max_CPU_time>\n");
        return EINVAL;
    }
   // printf(args[1][0]);

    //chnages the scheduling policy
    if (args[1][0] == 's') {
        cmd5 = 1;
    }
    if (args[1][0] == 'f') {
        cmd5 = 2;
    }
    if (args[1][0] == 'p') {
        cmd5 = 3;
    }
    
    //tunrs args into ints to be used
    sscanf(args[2], "%d", &numJobs);
    sscanf(args[3], "%d", &pri);
    sscanf(args[4], "%d", &cpuTime);
    sscanf(args[5], "%d", &maxCpuTime);

    
    //runs the test function
    test(cmd5, numJobs, pri, cpuTime, maxCpuTime);

}

/*
 * The quit command.
 */
int cmd_quit(int nargs, char **args) {
    quit();
    exit(0);
}


//help list
int cmd_helpmenu(int n, char **a) {
    int cmd3 = 5;
    printf("%s, \n%s, \n%s", "run <job> <time> <pri>: submit a job named <job>", "\t\t\texecution time is <time>", "\t\t\tpriority is <pri>.");
    printf("\n%s", "list: display the job status.");
    printf("\n%s", "fcfs: change the scheduling policy to FCFS.");
    printf("\n%s", "sjf: change the scheduling policy to SJF.");
    printf("\n%s", "priority: change the scheduling policy to priority.");
    printf("\n%s", "test <benchmark> <policy> <num_of_jobs> <priority_levels>");
    printf("\n%s", "     <min_CPU_time> <max_CPU_time>");
    printf("\n%s", "quit: exit CSUBatch\n");
    switchCommand(cmd3);
    return 0;
}

/*
 *  Command table.
 */
static struct {
    const char *name;
    int (*func)(int nargs, char **args);
} cmdtable[] = {
    /* commands: single command must end with \n */
    { "?\n", cmd_helpmenu},
    { "h\n", cmd_helpmenu},
    { "help\n", cmd_helpmenu},
    { "r", cmd_run},
    { "run", cmd_run},
    { "q\n", cmd_quit},
    { "quit\n", cmd_quit},
    { "list\n", cmd_list},
    { "l\n", cmd_list},
    { "fcfs\n", cmd_changePolicy},
    { "sjf\n", cmd_changePolicy},
    { "p\n",  cmd_changePolicy},
    { "priority\n", cmd_changePolicy},
    { "test", cmd_test}

   
};

/*
 * Process a single command.
 */
int cmd_dispatch(char *cmd) {
   
    time_t beforesecs, aftersecs, secs;
    u_int32_t beforensecs, afternsecs, nsecs;
    char *args[MAXMENUARGS];
    int nargs = 0;
    char *word;
    char *context;
    int i, result;
    void * Dispatcher(void * arg);

    for (word = strtok_r(cmd, " ", &context);
            word != NULL;
            word = strtok_r(NULL, " ", &context)) {

        if (nargs >= MAXMENUARGS) {
            printf("Command line has too many words\n");
            return E2BIG;
        }
        args[nargs++] = word;
    }

    if (nargs == 0) {
        return 0;
    }

    for (i = 0; cmdtable[i].name; i++) {
        if (*cmdtable[i].name && !strcmp(args[0], cmdtable[i].name)) {
            assert(cmdtable[i].func != NULL);

            /* Call function through the cmd_table */
            result = cmdtable[i].func(nargs, args);
            return result;
        }
    }

    printf("%s: Command not found\n", args[0]);
    return EINVAL;
}

//command line loop
int commandLineRun()
{
	char *buffer;
        size_t bufsize = 64;
        
        buffer = (char*) malloc(bufsize * sizeof(char));
        if (buffer == NULL) {
 		perror("Unable to malloc buffer");
 		exit(1);
	}
 
    while (1) {
		printf("> [Help for menu]: ");
		getline(&buffer, &bufsize, stdin);
		cmd_dispatch(buffer);
                buffer = (char*) malloc(bufsize * sizeof (char));
	}
        return 0;
}

//switches the current command based on user input
void switchCommand(int command) {

    if (command == 1) {

        CurrentCommand = Shortest;
    } else if (command == 2) {
        CurrentCommand = FCFS;
    } else if (command == 3) {
        CurrentCommand = Priority;
    } else if (command == 4) {

        CurrentCommand = Run;
    } else if (command == 5) {
        CurrentCommand = Help;
    }

    pthread_cond_signal(&condition1);



}

//signals the scheduler to start running
void signalProcesses() {
    pthread_cond_signal(&condition1);
}

//FCFS schedulign function
void ScheduleFCFS() {
    //changes command
    CurrentScheduleFunction = FirstComeFirstServe;
    //init temp arrays
    struct Job tempArray[5];
    struct Job newArray[5];

    int indexToPop;

    //gathers the jobqueue jobs and stores them in temp array
    for (int k = 0; k < sizeof (jobQueue) / sizeof (struct Job); k++) {
        tempArray[k] = jobQueue[k];
    }

    struct Job blankJob = {0};

    //temp array gets sorted based on arrival time
    for (int i = 0; i < sizeof (tempArray) / sizeof (struct Job); i++) {
        struct Job temp = tempArray[0];
        indexToPop = 0;
        for (int j = 1; j < sizeof (tempArray) / sizeof (struct Job); j++) {

            if (temp.executionTime == 0 || temp.executed == true) {
                temp = tempArray[j];
                indexToPop = j;

            } else {
                double timeDifference = difftime(temp.timeMilliseconds, tempArray[j].timeMilliseconds);
                if (timeDifference < 0) {
                    temp = tempArray[j];
                    indexToPop = j;

                }
            }
        }
        newArray[i] = temp;
        tempArray[indexToPop] = blankJob;
    }
    //populates final array with the sorted temp array
    struct Job finalArray[sizeof (jobQueue) / sizeof (struct Job)];
    int indexNewElement = 0;
    for (int j = sizeof (newArray) / sizeof (struct Job) - 1; j >= 0; j--) {
        finalArray[indexNewElement] = newArray[j];
        indexNewElement++;
    }

    //repopulates the jobqueue with the sorted final array
    int jobQueueIndex = 0;
    for (int i = 0; i < sizeof (finalArray) / sizeof (struct Job); i++) {
        if (finalArray[i].executionTime != 0) {
            jobQueue[jobQueueIndex] = finalArray[i];
            jobQueueIndex++;
        }
    }
}

//compares the execution integers 
int sjfComparison(const void *a, const void *b) {
    struct Job *ia = (struct Job *) a;
    struct Job *ib = (struct Job *) b;
    return (ia->executionTime - ib->executionTime);
}
//comapres the priority values
int priorityComparison(const void *a, const void *b) {
    struct Job *ia = (struct Job *) a;
    struct Job *ib = (struct Job *) b;
    return (ia->priority - ib->priority);
}

//SJF Scheduling method
void sjfScheduling() {
    //changes command
    CurrentScheduleFunction = ShortestJobFirst;
    //tempArray init
    struct Job tempArray[5];
    //populates temp array with jobqueue
    for (int i = 0; i < sizeof (jobQueue) / sizeof (struct Job); i++) {
        tempArray[i] = jobQueue[i];
    }

    //sorts the array based on execution time
    int n = sizeof (tempArray) / sizeof (struct Job);
    qsort(tempArray, n, sizeof (struct Job), sjfComparison);

    int indexNewElement = 0;

    //repopulates jobqueue with sorted objects
    for (int j = 0; j < sizeof (tempArray) / sizeof (struct Job); j++) {
        jobQueue[indexNewElement] = tempArray[j];
        indexNewElement++;
    }

}
//priority scheudling method
void priorityScheduling() {
    //command switch
    CurrentScheduleFunction = PriorityOrdering;
    
    //temp array init
    struct Job tempArray[5];

//populates temp array with current jobqueue
    for (int i = 0; i < sizeof (jobQueue) / sizeof (struct Job); i++) {
        tempArray[i] = jobQueue[i];
    }

    //sorts the temp array based on priority
    int n = sizeof (tempArray) / sizeof (struct Job);
    qsort(tempArray, n, sizeof (struct Job), priorityComparison);

    int indexNewElement = 0;


    //repopulates job queue with sorted job list
    for (int j = 0; j < sizeof (tempArray) / sizeof (struct Job); j++) {
        jobQueue[indexNewElement] = tempArray[j];
        indexNewElement++;
    }
}

//takes out job from the array
void JobQueuePop() {
    int queueSize = sizeof (jobQueue) / sizeof (struct Job);
    for (int i = 0; i < queueSize; i++) {
        if (i != queueSize - 1) {
            jobQueue[i] = jobQueue[i + 1];
        } else {
            struct Job temp = {};
            jobQueue[i] = temp;
        }
    }

}


//returns whether the job queue is full 
int isJobQueueFull() {
    const int sizeOfQueue = sizeof (jobQueue) / sizeof (struct Job);
    for (int i = 0; i < sizeOfQueue; i++) {
        if (jobQueue[i].executionTime == 0) {
            return i;
        }

    }
    return -1;
}

//adds job to queue
void QueueAdd(struct Job job) {
    int element = isJobQueueFull();
    if (element != -1) {
        jobQueue[element] = job;
    } else {
        printf("\nQueue is full. Could not add job.\n");
    }
}

//find the waiting time of all jobs currently in job queue
void findWaitingTime(struct Job myArray[], int n,
        int wt[]) {
    // waiting time for first process is 0
    wt[0] = 0;

    // calculating waiting time
    for (int i = 1; i < n; i++)
        wt[i] = myArray[i - 1].executionTime + wt[i - 1];
}


//find the turnaround time of all jobs currently within job queue
void findTurnAroundTime(struct Job myArray[], int n,
        int wt[], int tat[]) {
    // calculating turnaround time by adding
    // executionTime[i] + wt[i]
    for (int i = 0; i < n; i++)
        tat[i] = myArray[i].executionTime + wt[i];
}

//find average times of jobs within job queue
float findavgTime(struct Job myArray[], int n) {
    int wt[n], tat[n], total_wt = 0, total_tat = 0;
    findWaitingTime(myArray, n, wt);
    findTurnAroundTime(myArray, n, wt, tat);
    float waitTime;

    for (int i = 0; i < n; i++) {
        total_wt = total_wt + wt[i];
        total_tat = total_tat + tat[i];
       
    }


    printf("\n%s", "Average turn around time: ");
    printf("%.3f]n", (float) total_tat / (float) n);


    printf("\n%s", "Average waiting time: ");
    printf("%.3f\n", (float) total_wt / (float) n);
 


}

//gets the current time for storage in the job structure 
char* get_time_string() {
    struct tm *tm;
    time_t t;
    char *str_time = (char *) malloc(100 * sizeof (char));
    t = time(NULL);
    tm = localtime(&t);
    strftime(str_time, 100, "%H:%M:%S", tm);
    return str_time;
}

//dispatcher module
void * Dispatcher(void * arg) {
    pid_t pid;
    char* args[3];
    char* executeTime[4];
    while(true){

        if(jobQueue[0].name == NULL){
            pthread_cond_wait(&condition2, &lock2);
        }
        else if (!schedulingInProgress){
            int executionTime = jobQueue[0].executionTime;

            sprintf(executeTime, "%d", executionTime);
            args[0] = jobQueue[0].name;
            args[1] = executeTime;
            args[2] = NULL;


            switch(pid = fork()){
                case -1:
                    perror("Fork has failed\n");
                    break;

                case 0:
                    execv(jobQueue[0].name, args);
                    //puts("Job could not be found\n");
                    exit(EXIT_FAILURE);
                    break;
                default:
                    jobQueue[0].executed = true;
                    start1 = clock();
                    //printf("waiting\n");
                    sleep(executionTime + 1);
                    end1 = clock();
                    cpu_time_used1 += ((double) (end1 - start1)) / CLOCKS_PER_SEC;
                    procCompleted += 1;
                    JobQueuePop();
                    //waitpid(pid, &status, 0);
                    break;
            }
        }
    }
    return NULL;
}
                           
//scheduling module
void * Scheduler(void * arg) {

    while (true) {
        switch (CurrentCommand) {

            case Run:
                switch (CurrentScheduleFunction) {

                    case FirstComeFirstServe: //No need to run scheduler algorithm after job is added if it is first come first served
                        CurrentCommand = NoCommand;
                        break;

                    case PriorityOrdering:
                        schedulingInProgress = true;
                        priorityScheduling();
                        schedulingInProgress = false;
                        break;

                    case ShortestJobFirst:
                        schedulingInProgress = true;
                        sjfScheduling();
                        schedulingInProgress = false;
                        break;
                }
                CurrentCommand = NoCommand;
                break;

            case FCFS:
                if (CurrentScheduleFunction != FirstComeFirstServe) {
                    CurrentScheduleFunction = FirstComeFirstServe;
                    schedulingInProgress = true;
                    ScheduleFCFS();
                    schedulingInProgress = false;
                    CurrentCommand = NoCommand;
                } else {
                    //printf("First-Come, First-Serve is already the current scheduling priority.");
                    CurrentCommand = NoCommand;
                }
                break;

            case Shortest:
                if (CurrentScheduleFunction != ShortestJobFirst) {
                    CurrentScheduleFunction = ShortestJobFirst;
                    schedulingInProgress = true;
                    sjfScheduling();
                    schedulingInProgress = false;
                    CurrentCommand = NoCommand;
                } else {
                    //printf("Shortest-Job First is already the current scheduling priority.");
                    CurrentCommand = NoCommand;
                }
                break;

            case Priority:
                if (CurrentScheduleFunction != PriorityOrdering) {
                    CurrentScheduleFunction = PriorityOrdering;
                    schedulingInProgress = true;
                    priorityScheduling();
                    schedulingInProgress = false;
                    CurrentCommand = NoCommand;
                } else {
                    //printf("Shortest-Job First is already the current scheduling priority.");
                    CurrentCommand = NoCommand;
                }
                break;

            case Help:
                CurrentCommand = NoCommand;
                pthread_cond_wait(&condition1, &lock1);
                break;

            case NoCommand:
                pthread_cond_wait(&condition1, &lock1);
                break;
        }
    }

}


//lists the current jobs in the queue
void list() {
    //init variables 
    char progress[5];
    char curFunction[10];
    char executionTimeStr[5];
    char priorityStr[5];
    int numJobs = 0;
    char name[10];
    
    //checks for which policy is currently being used by the scheduler
    if (CurrentScheduleFunction == FirstComeFirstServe) {
        strncpy(curFunction, "FCFS", 10);
    } else if (CurrentScheduleFunction == ShortestJobFirst) {
        strncpy(curFunction, "SJF", 10);
    } else if (CurrentScheduleFunction == PriorityOrdering) {
        strncpy(curFunction, "Priority", 10);
    }


    //counts how many jobs are currently within the job queue
    for (int k = 0; k < sizeof (jobQueue) / sizeof (struct Job); k++) {
        if (jobQueue[k].name != NULL) {
            numJobs += 1;

        }
    }
    
    
    //checks to make sure there is atleast one job in job queue
    if (jobQueue[0].name != NULL) {
        printf("\n%s %d\n", "Total number of jobs in the queue:", numJobs);
        printf("\n%s %s\n", "Scheduling Policy", curFunction);

        printf("%s %20.8s %.9s %5.9s %5.8s\n", "Name", "CPU_Time", "Priority", "Arrival_Time", "Progress\n");

        for (int c = 0; c < sizeof (jobQueue) / sizeof (jobQueue[0]); c++) {

            //prints whther a job is being ran or not by the dispatcher
            if (jobQueue[c].executed == true) {
                strncpy(progress, "Run", 5);
            } else {
                strncpy(progress, "", 5);
            }

            //prints job info formatted correctly 
            if (jobQueue[c].name != NULL) {
                sprintf(executionTimeStr, "%d", jobQueue[c].executionTime);
                sprintf(priorityStr, "%d", jobQueue[c].priority);
                
                
                printf("%-10.15s %11.3s %9.3s %11.7s %6.9s",
                        jobQueue[c].name,
                        executionTimeStr,
                        priorityStr,
                        *jobQueue[c].arrivalTime,
                        progress);
                printf("\n");

            }

        }

    } else {
        printf("\nThe Job Queue is Empty\n");
    }
    printf("\n");
}

//performance evalution
void test(int command, int numJobs, int intPri, int cpuMin, int cpuMax) {
    //init variables
    int n = sizeof (jobQueue) / sizeof (jobQueue[0]);
    struct Job tempArray[20];
    float throughput;

    //preset jobs for testing
    struct Job job1 = {"job1\0", 13, 1,
        {get_time_string()}, time(NULL), false,1};
    struct Job job2 = {"job2\0", 23, 2,
        {get_time_string()}, time(NULL), false,2};
    struct Job job3 = {"job3\0", 42, 3,
        {get_time_string()}, time(NULL), false,3};
    struct Job job4 = {"job4\0", 34, 4,
        {get_time_string()}, time(NULL), false,4};
    struct Job job5 = {"job5\0", 12, 5,
        {get_time_string()}, time(NULL), false,5};
    
    // tempArray gets populated
    tempArray[0] = job1;
    tempArray[1] = job2;
    tempArray[2] = job3;
    tempArray[3] = job4;
    tempArray[4] = job5;
    
    
    for (int o = 0; o <= numJobs; o++) {
        if (tempArray[o].name != NULL) {
            jobQueue[o] = tempArray[o];
        }
    }

    if (command == 1) {
        CurrentCommand = Shortest;
    } else if (command == 2) {
        CurrentCommand = FCFS;
    } else if (command == 3) {
        CurrentCommand = Priority;
    }

    pthread_cond_signal(&condition1);
    pthread_cond_signal(&condition2);

    printf("\n%s %d", "Total number of jobs submitted: ", numJobs);
    findavgTime(jobQueue, n);
    printCpu();
}
void printCpu(){
    float throughput;
    throughput = (float) procCompleted / (float) cpu_time_used1;
    tempT = throughput;

    printf("\n%s %.2f", "Average CPU time:", cpu_time_used1);
    printf("\n%s %.3f %s", "Throughput:", tempT, "No./second\n");
    procCompleted = 0;
}

void quit() {
    int numberOfJobs = 0;
    int n = sizeof (jobQueue) / sizeof (jobQueue[0]);
    for (int p = 0; p < n; p++) {
        if (jobQueue[p].name != NULL) {
            numberOfJobs += 1;

        }

    }
    printf("\n%s %d", "Total number of jobs submitted: ", numberOfJobs);
    findavgTime(jobQueue, n);

    printf("\n%s %.2f", "Average CPU time:", cpu_time_used1);
    printf("\n%s %.3f %s", "Throughput:", (float) procCompleted / (float) cpu_time_used1, "No./second");
    procCompleted = 0;
}

void run(char name[], int executionTime, int Priority) {
    int n;
    int numberJobs = 0;
    int total_wt = 0;
    char curFunction[10];
    if (CurrentScheduleFunction == FirstComeFirstServe) {
        strncpy(curFunction, "FCFS", 10);
    } else if (CurrentScheduleFunction == ShortestJobFirst) {
        strncpy(curFunction, "SJF", 10);
    } else if (CurrentScheduleFunction == PriorityOrdering) {
        strncpy(curFunction, "Priority", 10);
    }
    int index;
    bool runCmd = true;
    bool queueWasEmpty;
  
    if (jobQueue[0].name == NULL) {
        queueWasEmpty = true;
    } else {
        queueWasEmpty = false;
    }

    struct Job job = {name, executionTime, Priority,{get_time_string()}, time(NULL), false};
    index = 0;
    while(runCmd){
        
        if (jobQueue[index].executionTime == 0) {
            jobQueue[index] = job;
            runCmd = false;

        } else if (isJobQueueFull() == -1) {
            printf("\nJob Queue is Full!");
        } else {
         index += 1;
        }
    }
    printf(jobQueue[0].name);
    printf("\n%s %s %s", "Job", name, "was submitted\n");
    for (int m = 0; m < sizeof (jobQueue) / sizeof (jobQueue[0]); m++) {
        if (jobQueue[m].name != NULL) {
            numberJobs += 1;

        }
       
    }
 
    
    n = sizeof (jobQueue)/ sizeof(jobQueue[0]);
   
    for(int u = 1; u < n ; u++){
      
        total_wt += jobQueue[u].executionTime;
    }

    printf("%s %d\n", "Total number of jobs in the queue:", numberJobs);
    printf("%s %d\n", "Expected waiting time:", total_wt);
    printf("%s %s\n", "Scheduling Policy:", curFunction);


    
    if (queueWasEmpty) {
        pthread_cond_signal(&condition2); //If the queue is empty, then signal the Dispatcher to run the job
    } else {
        CurrentCommand = Run;
        pthread_cond_signal(&condition1);
    }

}

int main(int argc, char *argv[]) {
    pthread_t sID, dID;
    pthread_create(&sID, NULL, Scheduler, NULL);
    pthread_create(&dID, NULL, Dispatcher, NULL);
    char* currenttime = get_time_string();
    
    printf("Welcome to our batch job scheduler.\nType 'help' to find more about CSUBatch commands.\n");

    commandLineRun();



    pthread_create(&sID, NULL, Scheduler, NULL);
    pthread_create(&dID, NULL, Dispatcher, NULL);



    return 0;


}



