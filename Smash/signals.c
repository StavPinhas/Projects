// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */   
#define _POSIX_SOURCE 
#include "signals.h"
#include "commands.h"
#include <signal.h>
#include <linux/limits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <fcntl.h>

extern int fg_pid;
extern int fg_id;
extern char* fg_cmd;
extern JobList* jobs;

//CTRL C
void CTRL_C_Handler (int signum){
   removeFinishedJobs(jobs);
   fprintf(stdout,"smash: caught ctrl-C\n");
   int pid = fg_pid;
   if(!pid){
      return;
   }
   if(kill(pid, SIGKILL) == -1){
      perror("smash error: kill failed");
      return;
   }
   fprintf(stdout,"smash: process %d was killed\n", pid);
   fg_pid = 0;
}

//CTRL Z
void CTRL_Z_Handler(int signum){
   fprintf(stdout,"smash: caught ctrl-Z\n");
   removeFinishedJobs(jobs);
   int pid = fg_pid;
   if(!pid){
      return;
   }
   Job* returning_job = createJob(pid, ST, fg_cmd);
   insertJob(jobs, returning_job, fg_id);
   if(kill(pid, SIGSTOP) == -1){
      perror("smash error: kill failed");
      return;
   }
   fprintf(stdout,"smash: process %d was stopped\n", pid);
   fg_pid = 0;
}

