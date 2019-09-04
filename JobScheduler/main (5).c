#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX 256

enum Command{NoCommand ,FCFS, Shortest, Priority, Run, Help};
enum ScheduleType{FirstComeFirstServe, ShortestJobFirst, PriorityOrdering};

struct Job
{
    char *name;
    int executionTime;
    int priority;
    char* arrivalTime[100];
    time_t timeMilliseconds;
    bool executed;

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

int sjfComparison(const void *a, const void *b) {
    struct Job *ia = (struct Job *) a;
    struct Job *ib = (struct Job *) b;
    return (ia->executionTime - ib->executionTime);
}

int priorityComparison(const void *a, const void *b) {
    struct Job *ia = (struct Job *) a;
    struct Job *ib = (struct Job *) b;
    return (ia->priority - ib->priority);
}

void sjfScheduling() {
    CurrentScheduleFunction = ShortestJobFirst;
    struct Job tempArray[5];
    for (int i = 0; i < sizeof (jobQueue) / sizeof (struct Job); i++) {
        tempArray[i] = jobQueue[i];
    }

    int n = sizeof (tempArray) / sizeof (struct Job);
    qsort(tempArray, n, sizeof (struct Job), sjfComparison);

    int indexNewElement = 0;

    for (int j = 0; j < sizeof (tempArray) / sizeof (struct Job); j++) {
        jobQueue[indexNewElement] = tempArray[j];
        indexNewElement++;
    }

    printf("help2");
}

void ScheduleFCFS(){
    CurrentScheduleFunction = FirstComeFirstServe;
    struct Job tempArray[5];
    struct Job newArray[5];

    int indexToPop;
    for (int i = 0; i < sizeof(jobQueue)/sizeof(struct Job); i++ ){
        tempArray[i] = jobQueue[i];
    }

    struct Job blankJob = {0};

    for (int i = 0; i < sizeof(tempArray)/sizeof(struct Job); i++){
        struct Job temp = tempArray[0];
        indexToPop = 0;
        for (int j = 1; j < sizeof(tempArray)/sizeof(struct Job); j++){

            if(temp.executionTime == 0 || temp.executed == true){
                temp = tempArray[j];
                indexToPop = j;

            }
            else{
                double timeDifference = difftime(temp.timeMilliseconds, tempArray[j].timeMilliseconds);
                if (timeDifference < 0){
                    temp = tempArray[j];
                    indexToPop = j;

                }
            }
        }
        newArray[i] = temp;
        tempArray[indexToPop] = blankJob;
    }
    struct Job finalArray[sizeof(jobQueue)/sizeof(struct Job)];
    int indexNewElement = 0;
    for (int j = sizeof(newArray)/sizeof(struct Job)-1; j >= 0; j--){
        finalArray[indexNewElement] = newArray[j];
        indexNewElement++;
    }

    int jobQueueIndex = 0;
    for(int i = 0; i < sizeof(finalArray)/sizeof(struct Job); i++){
        if(finalArray[i].executionTime != 0){
            jobQueue[jobQueueIndex] = finalArray[i];
            jobQueueIndex++;
        }
    }
}

char* get_time_string()
{
    struct tm *tm;
    time_t t;
    char *str_time = (char *) malloc(100*sizeof(char));
    t = time(NULL);
    tm = localtime(&t);
    strftime(str_time, 100, "%H:%M:%S", tm);
    return str_time;
}

void priorityScheduling() {
    CurrentScheduleFunction = PriorityOrdering;
    struct Job tempArray[5];


    for (int i = 0; i < sizeof (jobQueue) / sizeof (struct Job); i++) {
        tempArray[i] = jobQueue[i];
    }

    int n = sizeof (tempArray) / sizeof (struct Job);
    qsort(tempArray, n, sizeof (struct Job), priorityComparison);

    int indexNewElement = 0;

    for (int j = 0; j < sizeof (tempArray) / sizeof (struct Job); j++) {
        jobQueue[indexNewElement] = tempArray[j];
        indexNewElement++;
    }
}

void findWaitingTime(struct Job myArray[], int n,
        int wt[]) {
    // waiting time for first process is 0
    wt[0] = 0;

    // calculating waiting time
    for (int i = 1; i < n; i++)
        wt[i] = myArray[i - 1].executionTime + wt[i - 1];
}

void findTurnAroundTime(struct Job myArray[], int n,
        int wt[], int tat[]) {
    // calculating turnaround time by adding
    // executionTime[i] + wt[i]
    for (int i = 0; i < n; i++)
        tat[i] = myArray[i].executionTime + wt[i];
}

void findavgTime(struct Job myArray[], int n) {
    int wt[n], tat[n], total_wt = 0, total_tat = 0;
    findWaitingTime(myArray, n, wt);
    findTurnAroundTime(myArray, n, wt, tat);
    printf("\n%s, %s", "Jobs  ", " Burst time  ");
    printf("%s, %s", " Waiting time  ", " Turn around time\n");
    for (int i = 0; i < n; i++) {
        total_wt = total_wt + wt[i];
        total_tat = total_tat + tat[i];
        printf("%s, %s, %s, %d, %s, %d, %s, %d", "   ", myArray[i].name, "\t\t",
                myArray[i].executionTime, "\t    ", wt[i],
                "\t\t  ", tat[i]);
    }

    printf("\n%s", "Average waiting time = ");
    printf("%.3f", (float) total_wt / (float) n);
    printf("\n%s", "Average turn around time = ");
    printf("%.3f", (float) total_tat / (float) n);
}

void help() {
    printf("%s, \n%s, \n%s", "run <job> <time> <pri>: submit a job named <job>", "\t\t\texecution time is <time>", "\t\t\tpriority is <pri>.");
    printf("\n%s", "list: display the job status.");
    printf("\n%s", "fcfs: change the scheduling policy to FCFS.");
    printf("\n%s", "sjf: change the scheduling policy to SJF.");
    printf("\n%s", "priority: change the scheduling policy to priority.");
    printf("\n%s", "test <benchmark> <policy> <num_of_jobs> <priority_levels>");
    printf("\n%s", "     <min_CPU_time> <max_CPU_time>");
    printf("\n%s", "quit: exit CSUBatch\n");
}

void JobQueuePop(){
    int queueSize = sizeof(jobQueue)/sizeof(struct Job);
    for(int i = 0; i < queueSize; i++){
        if(i != queueSize-1){
            jobQueue[i] = jobQueue[i+1];
        }
        else{
            struct Job temp = {};
            jobQueue[i] = temp;
        }
    }

}

int isJobQueueFull(){
    const int sizeOfQueue = sizeof(jobQueue)/sizeof(struct Job);
    for (int i = 0; i < sizeOfQueue; i++){
        if(jobQueue[i].executionTime == 0){
            return i;
        }

    }
    return -1;
}

void QueueAdd(struct Job job){
    int element = isJobQueueFull();
    if(element != -1){
        jobQueue[element] = job;
    }
    else{
        printf("\nQueue is full. Could not add job.\n");
    }
}

void * Dispatcher(void * arg){
    //int status;
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
                    puts("Job could not be found\n");
                    exit(EXIT_FAILURE);
                    break;
                default:
                    jobQueue[0].executed = true;
                    printf("waiting\n");
                    sleep(executionTime + 1);
                    JobQueuePop();
                    //waitpid(pid, &status, 0);
                    break;
            }
        }
    }
    return NULL;
}

void * Scheduler(void * arg){

    while(true){
        switch(CurrentCommand){

            case Run:
                switch (CurrentScheduleFunction){

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
                if(CurrentScheduleFunction != FirstComeFirstServe){
                    CurrentScheduleFunction = FirstComeFirstServe;
                    schedulingInProgress = true;
                    ScheduleFCFS();
                    schedulingInProgress = false;
                    CurrentCommand = NoCommand;
                }
                else{
                    printf("First-Come, First-Serve is already the current scheduling priority.");
                    CurrentCommand = NoCommand;
                }
                break;

            case Shortest:
                if(CurrentScheduleFunction != ShortestJobFirst){
                    CurrentScheduleFunction = ShortestJobFirst;
                    schedulingInProgress = true;
                    sjfScheduling();
                    schedulingInProgress = false;
                    CurrentCommand = NoCommand;
                }
                else{
                    printf("Shortest-Job First is already the current scheduling priority.");
                    CurrentCommand = NoCommand;
                }
                break;

            case Priority:
                if(CurrentScheduleFunction != PriorityOrdering){
                    CurrentScheduleFunction = PriorityOrdering;
                    schedulingInProgress = true;
                    priorityScheduling();
                    schedulingInProgress = false;
                    CurrentCommand = NoCommand;
                }
                else{
                    printf("Shortest-Job First is already the current scheduling priority.");
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
    return NULL;
}

void run(char name[], int executionTime, int Priority) {
    int index = 0;
    bool queueWasEmpty;
    //printf("%s", name);

    //char *name;
    if(jobQueue[0].name == NULL){
        queueWasEmpty = true;
    }
    else{
        queueWasEmpty = false;
    }

    struct Job job = {name, executionTime, Priority, {get_time_string()}, time(NULL), false};

        if (jobQueue[index].executionTime == 0) {
            jobQueue[index] = job;

        } else if (isJobQueueFull() == -1) {
            printf("\nJob Queue is Full!");
        }
        else{
            index+=1;
        }
      printf("%s\n", jobQueue[0].name);
            printf("%d\n", jobQueue[0].executionTime);
            printf("%d\n", jobQueue[0].priority);
            printf("%s\n", *jobQueue[0].arrivalTime);
            printf("\n");

        if(queueWasEmpty){
            pthread_cond_signal(&condition2); //If the queue is empty, then signal the Dispatcher to run the job
        }
        else{
            CurrentCommand = Run;
            pthread_cond_signal(&condition1); //If the queue is not empty, then the Dispatcher should already be running, so signal the Scheduler to sort the jobs
        }
}

void list(){
//    int empty = 0;
 for (int c = 0; c < sizeof (jobQueue) / sizeof (jobQueue[0]); c++) {

        if(jobQueue[c].executionTime != 0){
            printf("%s\n", jobQueue[c].name);
            printf("%d\n", jobQueue[c].executionTime);
            printf("%d\n", jobQueue[c].priority);
            printf("%s\n", *jobQueue[c].arrivalTime);
            printf("\n");
        }


    }
    printf("\n");
}

int main(void)
{
    pthread_t sID, dID;

    struct Job job1 = {"process\0", 5, 5, {get_time_string()}, time(NULL), false};
//    sleep(2);
//    struct Job job2 = {"job2\0", 5, 5, {get_time_string()}, time(NULL), false};
//    sleep(2);
//    struct Job job3 = {"job3\0", 5, 5, {get_time_string()}, time(NULL), false};
//    sleep(2);
//    struct Job job4 = {"job4\0", 5, 5, {get_time_string()}, time(NULL), false};

    jobQueue[0] = job1;
//    jobQueue[1] = job1;
//    jobQueue[2] = job2;
//    jobQueue[3] = job4;

    printf("Creating scheduler thread\n");

    if ( pthread_create( &sID, NULL, Scheduler, NULL) ) {
        printf("error creating thread.\n");
        abort();
    }

    printf("Creating dispatcher thread\n");
 ;
    if ( pthread_create( &dID, NULL, Dispatcher, NULL) ) {
        printf("error creating thread.\n");
        abort();
    }
    printf("Welcome to our batch job scheduler.\nType 'help' to find more about CSUBatch commands.\n");

    pthread_join(sID, NULL);
    pthread_join(dID, NULL);

    return 0;


}
