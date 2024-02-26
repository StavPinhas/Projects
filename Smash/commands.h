#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_LINE_SIZE 80
#define MAX_ARG 20


/*enums*/
/*******************************************************************************/
enum {FG, BG, ST}; //job run states
enum{NOT_A_NUMBER,NEGATIVE,POSITIVE}; //number options, '0' includes in positive
/*******************************************************************************/

/*structs*/
/*******************************************************************************/
//struct representing a single job
typedef struct Job {
    int id;           
    int pid;          
    int run_status;    // running status (0 - foreground, 1 - background, 2 - stopped)
    time_t start_time; // time in which the job was created
    char* cmd;       
    struct Job* prev;
    struct Job* next;
} Job;

// struct representing a list of jobs
typedef struct JobList {
    Job* head;  
    Job* tail;  
} JobList;
/*******************************************************************************/
/*functions*/
/*******************************************************************************/
//given functions
int ExeCmd(JobList* jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString);
int BgCmd(char* lineSize, JobList* jobs);

//aux functions
int isNumber(char* str);
bool isDigit(char c);
bool is_terminated(pid_t process_id);
bool Is_BuiltIn (char* cmd);

//job functions
JobList* createJobList();
Job* createJob(int pid, int run_status, char* cmd);
Job* isJobExist(JobList* list, int id);
void insertJob(JobList* list, Job* newJob,int id);
void deleteJob(JobList* list, int id, bool destroy);
void destroyJob(Job* job);
void displayJobs(JobList* list);
void freeList(JobList* list);
void removeFinishedJobs(JobList* list);
/*******************************************************************************/
#endif

