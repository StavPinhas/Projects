/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#define _POSIX_SOURCE
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "signals.h"
#include "commands.h"

#define MAX_LINE_SIZE 80
#define MAXARGS 20

char* L_Fg_Cmd;
JobList* jobs; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
char lineSize[MAX_LINE_SIZE]; 
extern int fg_pid;

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);
    char cmdString[MAX_LINE_SIZE]; 	   
	jobs = createJobList();
	
	//signal declaretions
	struct sigaction act_sigtstp = {0};
    struct sigaction act_sigint = {0};
    act_sigtstp.sa_handler = &CTRL_Z_Handler;
    act_sigint.sa_handler = &CTRL_C_Handler;
    act_sigtstp.sa_flags = SA_RESTART;
    act_sigint.sa_flags = SA_RESTART;
    sigfillset(&act_sigtstp.sa_mask);
    sigfillset(&act_sigint.sa_mask);
    if(sigaction(SIGTSTP, &act_sigtstp, NULL) == -1) {
    	fprintf(stderr, "smash error: SIGSTOP failed\n");
    }
    if(sigaction(SIGINT, &act_sigint, NULL) == -1) {
        fprintf(stderr, "smash error: SIGKILL failed\n");
    }
	

	/************************************/
	// Init globals 


	
	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL) 
			exit (-1); 
	L_Fg_Cmd[0] = '\0';
	
    	while (1)
    	{
	 	printf("smash > ");
		removeFinishedJobs(jobs);
		fg_pid = 0;
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';
		// background command
	 	if(BgCmd(lineSize, jobs) == 0){
		 	continue;
		}
		// built in commands
		int result = ExeCmd(jobs, lineSize, cmdString);
		if(result == -1){
			break;
		}
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}

