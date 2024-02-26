//		commands.c
//********************************************
#define _GNU_SOURCE
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
#define MAX_PATH_SIZE 4096

//contains the last directory path
char last_dir[MAX_PATH_SIZE];

//stats of the current job that runs in fg
int fg_pid = 0;
int fg_id;
char* fg_cmd;

//global varible of the current list of jobs
extern JobList* jobs;


//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(JobList* list, char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
    cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
	}
	if(list == NULL){
		return -1;
	}
	removeFinishedJobs(list);
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{
		//multiple arguments - error (we can assume atleast one argument)
		if(num_arg > 1){
			fprintf(stderr,"smash error: cd: too many arguments\n");
			return 0;
		}
		//request to go to the last directory
		else if(strcmp(args[1],"-") == 0){

			//no last directory
			if(last_dir[0] == '\0'){
				fprintf(stderr,"smash error: cd: OLDPWD not set\n");
				return 0;
			}
			else{ //there is a former directory
				char* tmp = get_current_dir_name();
				if(tmp == NULL){
					perror("smash error: getcwd failed");
					return 0;
				}
				if (chdir(last_dir) != 0) {
       				perror("smash error: chdir failed");
					free(tmp);
					return 0;
				}
				//success on changing directory, saving the former dir as last dir
				strcpy(last_dir,tmp);
				free(tmp);
			}
		}
		else{ //there is one argument, different from '-'
			char* tmp = get_current_dir_name();
			if(tmp == NULL){
					perror("smash error: getcwd failed");
					return 0;
			}
			if (chdir(args[1]) != 0) {
       				perror("smash error: chdir failed");
					free(tmp);
					return 0;
			}
			//success on changing directory, saving the former dir as last dir
			strcpy(last_dir,tmp);
			free(tmp);
		}
		return 0;

	} 
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		char* wd = get_current_dir_name();
		if(wd == NULL){
			perror("smash error: getcwd failed");
			return 0;
		}
		fprintf(stdout,"%s\n", wd);
		free(wd);
		
	}
	/*************************************************/
	else if (!strcmp(cmd, "jobs")) 
	{
		displayJobs(list);
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		int pid = getpid();
		fprintf(stdout,"smash pid is %d\n", pid);
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		Job* fg_job;
		if(num_arg == 0){ 
		//need to pick the job with the highest id in the list
			if(list->head == NULL){ //list is empty
				fprintf(stderr, "smash error: fg: jobs list is empty\n");
				return 0;
			}
			//list is not empty, tail has the highest job id
			fg_job = list->tail;
		}
		else if((num_arg > 1) || (isNumber(args[1]) != POSITIVE)){
			fprintf(stderr, "smash error: fg: invalid arguments\n");
			return 0;
		}
		
		else{ //there is an argument that is not negative
			int id = atoi(args[1]);
			//check if atoi failed or invalid id
			if(id == 0){
				if(args[1][0] != '0'){
					fprintf(stderr, "smash error: atoi failed\n");
				}
				else{ //we got '0' as an argument, it's not a valid job id
					fprintf(stderr, "smash error: fg: invalid arguments\n");
				}
				return 0;
			}
			fg_job = isJobExist(list,id);
			if(fg_job == NULL){ //job does not exist in the list
				fprintf(stderr, "smash error: fg: job-id %d does not exist\n", id);
				return 0;
			}
		}
		fprintf(stdout,"%s : %d\n",fg_job->cmd, fg_job->pid);
		if(kill(fg_job->pid, SIGCONT) == -1){ //kill failed
			perror("smash error: kill failed");
			return 0;
		}
		//kill succeed, update varibles
		fg_cmd = fg_job->cmd;
		fg_id = fg_job->id;
		fg_pid = fg_job->pid;

		//remove job from the list
		deleteJob(list, fg_job->id, false);
		if(waitpid(fg_job->pid, NULL,WUNTRACED) == -1){
			perror("smash error: waitpid failed");
			return 0;
		}
		//job has finished, hence reseting the fg_pid varible
		fg_pid = 0;
		return 0;
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
  		Job* bg_job;
		if(num_arg == 0){
			if(list->head == NULL){ //list is empty
				fprintf(stderr, "smash error: bg: jobs list is empty\n");
				return 0;
			}
			//list is not empty, searching the stopped process with the highest id
			bg_job = list->tail;
			while((bg_job != NULL) && (bg_job->run_status!=ST)){
				bg_job = bg_job->prev;
			}
			if(bg_job == NULL){
				fprintf(stderr, "smash error: bg: there are no stopped jobs to resume\n");
				return 0;
			}	
		}
		else if(((num_arg > 1) || isNumber(args[1]) != POSITIVE)){
			fprintf(stderr, "smash error: bg: invalid arguments\n");
			return 0;
		}
		else{ //there is an argument that is not negative
			int id = atoi(args[1]);

			//check if atoi failed or invalid id
			if(id == 0){
				if(!strcmp(args[1], "0")){
				fprintf(stderr, "smash error: atoi failed\n");
				}
				else{ //we got '0' as an argument, it's not a valid job id
					fprintf(stderr, "smash error: bg: invalid arguments\n");
				}
				return 0;
			}
		
			else{
				bg_job = isJobExist(list,id);
				if(bg_job == NULL){ //job does not exist in the list
					fprintf(stderr, "smash error: bg: job-id %d does not exist\n", id);
					return 0;
				}
				if(bg_job->run_status != ST){
					fprintf(stderr, "smash error: bg: job-id %d is already running in the background\n", id);
					return 0;
				}
			}
		}
			fprintf(stdout,"%s : %d\n",bg_job->cmd, bg_job->pid);
			if(kill(bg_job->pid, SIGCONT) == -1){ //kill failed
				perror("smash error: kill failed");
				return 0;
			}
			//kill succeed, update job's run status
			bg_job->run_status = BG;

			return 0;
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
   		if(num_arg != 1 || strcmp(args[1],"kill")){
			return -1;
		}
		if(list == NULL || list->head == NULL){
			return -1;
		}
		//the cmd is "quit kill" and we got a not empty list
		Job* curr_job = list->head;
		//looping on all the jobs in the list
		while(curr_job != NULL){
			time_t start_time = time(NULL);
			if(kill(curr_job->pid,SIGTERM) == -1){
				perror("smash error: kill failed");
				return -1;
			}
			fprintf(stdout,"[%d] %s - Sending SIGTERM...",curr_job->id, curr_job->cmd);
		
			if(is_terminated(curr_job->pid)){
				fprintf(stdout," Done.\n");
			}
			else{ //checking if ended within 5 seconds
				while(difftime(time(NULL),start_time) < 5){}
				if(is_terminated(curr_job->pid)){
					fprintf(stdout," Done.\n");
				}
				//didn't end in time, sending SIGKILL
				else{ 
					if(kill(curr_job->pid,SIGKILL) == -1){
						perror("smash error: kill failed");
					}
					fprintf(stdout," (5 sec passed) Sending SIGKILL... Done.\n");
				}
			}
			curr_job = curr_job->next;
		}
		return -1;
	} 
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		if((num_arg != 2) || (isNumber(args[1])) != NEGATIVE || (isNumber(args[2]))!=POSITIVE){
			fprintf(stderr, "smash error: kill: invalid arguments\n");
			return 0;
		}
		int id = atoi(args[2]);
		if(id == 0){
			fprintf(stderr, "smash error: kill: invalid arguments\n");
			return 0;
		}
		//if exist returns a pointer to job, else returns NULL
		Job *job = isJobExist(list,id); 
		if(job == NULL){
			fprintf(stderr, "smash error: kill: job-id %d does not exist\n", id);
			return 0;
		}
		//pointer to the string after the '-'
		int signum = atoi((args[1])+1); 
		//check if atoi failed
		if((signum == 0) && args[1][1]!= '0'){
			fprintf(stderr, "smash error: atoi failed\n");
			return 0;
		}
		pid_t pid = job->pid;
		if(kill(pid, signum) == -1){ 
			perror("smash error: kill failed");
			return 0;
		}
		else{ //kill succeeded
			printf("signal number %d was sent to pid %d\n",signum ,pid);
			return 0;
		}

	} 
	/*************************************************/	
	else if (!strcmp(cmd, "diff"))
	{
   		if(num_arg != 2){
			fprintf(stderr, "smash error: diff: invalid arguments\n");
			return 0;
		}
		//opening the 2 files
		int f1 = open(args[1] , O_RDONLY);
		if(f1 == -1){
			fprintf(stderr, "open() failed\n");
			return 0;
		}
		

		int f2 = open(args[2] , O_RDONLY);
		if(f2 == -1){
			fprintf(stderr, "open() failed\n");
			close(f1);
			return 0;
		}
		//success in opening files, comparing a single char each time
		char buf1, buf2;
		int r1 = read(f1, &buf1, 1);
		int r2 = read(f2, &buf2, 1);
		int diff = 1;
		while(( r1 == 1) && (r2 == 1)){
			if(buf1 != buf2){
			   fprintf(stdout,"%d\n", diff);
			   close(f1);
			   close(f2);
			   return 0;
			}
		r1 = read(f1, &buf1, 1);
		r2 = read(f2, &buf2, 1);
		}
		close(f1);
		close(f2);
		if((r1 == -1) || (r2 == -1)){
			fprintf(stderr, "open() failed\n");
			return 0;	
		}
	
		if(r1 != r2){
			fprintf(stdout,"%d\n", diff);
			return 0;
		}
		//files idenctical (same content and length)
		diff = 0;
		fprintf(stdout,"%d\n", diff);
		return 0;

	} 
	/*************************************************/	
	else // external command
	{
 		ExeExternal(args, cmdString);
	 	return 0;
	}
    return 0;
}

//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID;
    	switch(pID = fork()) 
	{
    		case -1: 
				//fork failed
				perror("smash error: fork failed");
				break;

        	case 0 :
                	// Child Process
               		setpgrp();
					if(execvp(args[0], args) == -1){
						perror("smash error: execvp failed");
						exit(0);
					}
					break;


			default:
				fg_cmd = cmdString;
				fg_pid = pID;
				if(jobs == NULL)
					exit(0);
				
				if(jobs->tail == NULL){
					fg_id = 1;
				}
				else{
					fg_id = (jobs->tail->id)+1;
				}
				if(waitpid(pID, NULL, WUNTRACED) == -1){
					perror("smash error: waitpid failed");
				}

	}
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, JobList* jobs)
{

	char* Command;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	char Command_new[MAX_LINE_SIZE+1];
	int Cmd_len = strlen(lineSize);
	if (lineSize[Cmd_len-2] == '&')
	{
		strcpy(Command_new,lineSize);
		Command_new[Cmd_len-1] = '\0';
		lineSize[Cmd_len-2] = '\0';
		Command = strtok(lineSize, delimiters);
		if(Command == NULL){
			return 0; 
		}
		args[0] = Command;
		int num_arg = 0;
		for(int i = 1; i<MAX_ARG; i++){
			args[i] = strtok(NULL, delimiters);
			if(args[i] != NULL){
				num_arg++;
			}
		}
		if(Is_BuiltIn(Command)){
			return -1;
		}
		int pID;
		Job* new_job;
		switch(pID = fork()) {
    		case -1: 
				//fork failed
				perror("smash error: fork failed");
				break;

        	case 0 :
                	// Child Process
               		setpgrp();
					if(execvp(args[0], args) == -1){
						perror("smash error: execvp failed");
						exit(0);
					}
					break;


			default:
				new_job = createJob(pID, BG, Command_new);
				if(jobs == NULL)
					exit(0);
				insertJob(jobs, new_job, 0);
				break;
		}	

		return 0;
	}

	return -1;
}

//checks if a given string is a number. returns 0 - not a number, 1 - negative, 2 - positive or zero
int isNumber(char* str){
	int i = 0;
	int result = 2;

	if(str[i] == '-'){
		i++;
		result = 1;
	}

	if(str[i] == '\0'){
		result = 0;
		return result;
	}

	while(str[i] != '\0'){
		if(!isDigit(str[i])){
			result = 0;
			return result;
		}
		i++;
	}

	return result;

}

bool isDigit(char c){
	return ((c>='0') && (c<='9'));
}


//checks if the process is terminated
bool is_terminated(pid_t process_id){
    waitpid(process_id, NULL, WNOHANG); 
    return (kill(process_id, 0) != 0);
}

//checks if the command is a built in command
bool Is_BuiltIn (char* cmd){
	 if (!strcmp(cmd, "cd")) return  true;
	 if (!strcmp(cmd, "pwd")) return  true;
	 if (!strcmp(cmd, "jobs")) return  true;
	 if (!strcmp(cmd, "quit")) return  true;
	 if (!strcmp(cmd, "diff")) return  true;
	 if (!strcmp(cmd, "bg")) return  true;
	 if (!strcmp(cmd, "kill")) return  true;
	 if (!strcmp(cmd, "fg")) return  true;
	 if (!strcmp(cmd, "showpid")) return  true;
	 return false;
 }

 // function to allcoate a job list
JobList* createJobList() {
    JobList* list = (JobList*)malloc(sizeof(JobList));
    if (list == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    return list;
}

// function to allocate a new job
Job* createJob(int pid, int run_status, char* cmd) {
    Job *newJob = (Job*)malloc(sizeof(Job));
    if (newJob == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    newJob->id = 0;
    newJob->pid = pid;
    newJob->run_status = run_status;
    newJob->cmd = (char*)malloc(strlen(cmd) + 1);
    if(newJob->cmd == NULL){
       fprintf(stderr, "Error: Memory allocation failed\n");
       free(newJob);
       return NULL;
    }
    strcpy(newJob->cmd, cmd);
    newJob->prev = NULL;
    newJob->next = NULL;
    return newJob;
}

// function to insert a job into the list in increasing order of job ID
//if id !=0 it means that the job was already in the list, and will return to his location
//else, the job is new in the list and will go to the tail with the highest id in the list + 1.
void insertJob(JobList* list, Job *newJob, int id) {
	removeFinishedJobs(list);
    if (list == NULL) {
        return;
    }
	//list is empty
    if (list->head == NULL) {
		newJob->start_time = time(NULL);
        list->head = newJob;
        list->tail = newJob;
		if(id == 0){
		   newJob->id = 1;
		}
		else{
			newJob->id = id;
		}
        return;
    }
	//new job - inserting to the tail of the list
	if(id == 0){
		int curr_max_id = list->tail->id;
    	newJob->id = curr_max_id + 1;
		newJob->prev = list->tail;
    	list->tail->next = newJob;
    	list->tail = newJob;
	}
	//a returning job - inserting to it's previous location in the list
	else{
		newJob->id = id;
		Job* temp = list->head;
		while(temp != NULL && temp->id < id){
			temp = temp->next;
		}
		//supposed to go to the tail - all previous jobs have a lower id
		if(temp == NULL){
			newJob->prev = list->tail;
    		list->tail->next = newJob;
    		list->tail = newJob;
			newJob->next = NULL;
		}
		//inserting in the correct location
		else{
			//supposed to go to the start of the list (has the lowest id)
			if(temp == list->head){
				newJob->prev = NULL;
				newJob->next = temp;
				list->head = newJob;
				temp->prev = newJob;
			}
			else{
			newJob->prev = temp->prev;
			newJob->next = temp;
			(temp->prev)->next = newJob;
			temp->prev = newJob;
			}
		}

	}
	//updating start time
    newJob->start_time = time(NULL);

}

// function to delete a job from the list
void deleteJob(JobList* list, int id, bool destroy) {
	// no list
    if (list == NULL) {
        fprintf(stderr, "Error: No List\n");
        return;
    }
	//list is empty
    if (list->head == NULL) {
        fprintf(stderr, "Error: List is empty\n");
        return;
    }

    Job *current = list->head;
    while (current != NULL && current->id != id) {
        current = current->next;
    }
	//id does not exisst in list
    if (current == NULL) {
        fprintf(stderr, "Error: Job with ID %d not found\n", id);
        return;
    }
	//no the first element
    if (current->prev != NULL) {
        current->prev->next = current->next;
    } else {
		//deleting the first element. updating head
        list->head = current->next;
    }
	//not last element
    if (current->next != NULL) {
        current->next->prev = current->prev;
    }
	//deleting last element. updating tail
	else {
		list->tail = current->prev;
	}
    if(destroy)
    destroyJob(current);
}

// function to destroy a single job
void destroyJob(Job* job) {
    if (job == NULL) {
        return;
    }
    if(job->cmd == NULL){
        free(job);
        return;
    }
	//free memory of the command string
    free(job->cmd);  
	//free the memory of the job structure
    free(job);       
}

// function to display all jobs in the list
void displayJobs(JobList* list) {
	removeFinishedJobs(list);
    if (list == NULL) {
        //no list
        return;
    }
    if (list->head == NULL) {
        //list is empty
        return;
    }
    Job *current_job = list->head;
    while (current_job != NULL) {
        time_t curr_time = time(NULL);
        if(curr_time == (time_t)(-1)){
            perror("smash error: time failed");
            return;
        }
        double time_elapsed = difftime(curr_time, current_job->start_time);
        //print for background job
        if(current_job->run_status == BG){
        fprintf(stdout, "[%d] %s : %d %.0lf secs\n", current_job->id, current_job->cmd, current_job->pid, time_elapsed);
        }
        //job in stopped status
        else{
            fprintf(stdout, "[%d] %s : %d %.0lf secs (stopped)\n", current_job->id, current_job->cmd, current_job->pid, time_elapsed);
        }
        current_job = current_job->next;
    }
}

// function to free all memory allocated for the list
void freeList(JobList* list) {
    if(list == NULL){
        return;
    }
    Job *current = list->head;
    while (current != NULL) {
        Job *tmp = current;
        current = current->next;
        destroyJob(tmp);
    }
    free(list);
}

void removeFinishedJobs(JobList* list){
    if(list == NULL){
        return;
    }
    Job *current = list->head;
    while (current != NULL) {
        pid_t pid = waitpid(current->pid, &(current->run_status), WNOHANG| WUNTRACED | WCONTINUED); //returns not 0 only for terminated processes
        if(pid != 0){ 
			if(WIFSTOPPED(current->run_status)){
				current->run_status = ST;
				current = current->next;
			}
			else if (WIFCONTINUED(current->run_status)){
				current->run_status = BG;
				current = current->next;
			}
			else{ //removing finished process
            	if (current->prev == NULL){ //current is the head of the list
                	if(current->next == NULL){ //current is the only process on the list
                    	deleteJob(list, current->id, true);
                    	current = NULL;
                	}
                	else{ 
                    	current = current->next; //go to the next process
                   	 deleteJob(list,current->prev->id,true); //delete the finished process
                	}
                
            	}
            	else{ //current isn't the head of the list
              	  current = current->prev; //so we won't lose our place in the list
               	  deleteJob(list, current->next->id,true);//delete the finished process
               	  current = current->next; //check the next process after the deleted one
           	 	}
       	 	}
		}
		else{
			current = current->next;
		}
   	}
}

//checks if a job with the given id is in the list. if not - returns null, if yes - returns a pointer to the job.
Job* isJobExist(JobList* list, int id) {
    if (list == NULL) {
        return NULL;
    }
    if (list->head == NULL) {
        return NULL;
    }

    Job *current = list->head;
    while (current != NULL && current->id != id) {
        current = current->next;
    }

    return current;
}

