#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>/*this is for pid_t*/
#include <unistd.h>/*this is for getcwd*/
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"/*include declarations for parse-related structs*/

enum
BUILTIN_COMMANDS { NO_SUCH_BUILTIN=0, EXIT, JOBS, CD, HISTORY, KILL, HELP};
 
char* hist[1024]; /*array to hold the history of commands */
char* jobs[1024]; /*array to hold background jobs*/
int i = 0; /*for use in loops*/
int c = 0; /*for general tracking*/

char *
buildPrompt(){
        /* we allocate space for the file path that
                realistically shouldn't be longer than 256 
                characters and then fill it with getcwd
                which is a system call to get the current
                working directory */
  char* buff = malloc(260);
  getcwd(buff, 260);
  return strcat(buff, ":: ");
        /* ":: " gets concatenated onto the end before
                returning it solely for eye pleasure */
}
 
int
isBuiltInCommand(char * cmd){
  
  if ( strcmp(cmd, "exit") == 0){
    return EXIT;
  }
  else if ( strcmp(cmd, "cd") == 0){
    return CD;
  }
  else if ( strcmp(cmd, "jobs") == 0){
    return JOBS;
  }
  else if ( strcmp(cmd, "history") == 0){
    return HISTORY;
  }
  else if ( strcmp(cmd, "kill") == 0){
    return KILL;
  }
  else if ( strcmp(cmd, "help") == 0){
    return HELP;
  }
  return NO_SUCH_BUILTIN;
}

/*******************CD*****************/
void change_dir(char * path){
        if('~' == path[0]){ /*for home dir*/
                chdir("home");
                printf("home\n");
        }else{
                chdir(path);
        }
}

/****************HistoryList***********/
/* to display the history list*/
void getHistory(){
/*simply goes through the hist array and prints each one*/
        for( i=0 ; i<1024; i++ ){
                if(hist[i] != NULL){
                        printf("%d %s \n", i, hist[i]);
                }
        }
}

/******************HELP***************/
/*prints the help page*/
void help(){
        printf("~~~~~KaBooM Shell, version 1.0~~~~~~~~~ \n");
        printf("These shell commands are not completely finished.  Type `help' to see this anytime. \n");
        printf("Currently semi-working: \n Change Directory [cd] (must contain an argument) \n Help [help] (you will get this page, silly) \n Background Jobs [jobs] (will print the list of background jobs) \n Background a Job [command &] (space optional, sends a job to the background, not actually working correct) \n History [history] (displays the past commands, excluding commands that start with a !) \n ");
        printf("Repeat a command in History [!# or !-#] (repeats a command from the list of history of commands) \n Exit [exit] (will exit out of the shell, unless there are running background jobs, then it will alert you and ask you are sure about exiting) \n");
        printf("Kill a background job [kill #] (will kill a job that is on the list of background jobs. \n");
}


/*******************Jobs*********************/
/*this is the function that prints the running background jobs*/
void getJobs(){
        c = 0;
        for( i=0 ; i<1024; i++ ){
                if(jobs[i] != NULL){
                        c = 1;
                }
        }
        if(0 == c){
                printf("No background jobs.\n");
        }else if(1 == c){
                printf("PID Command \n");
                for( i=0 ; i<1024; i++ ){
                        if(jobs[i] != NULL){
                                printf("[%d] %s \n", i, jobs[i]);
                        }
                }
        }
}

/*this is the function that puts the jobs in the background,
        pretend works */
void bgJob(){
                pid_t pid;
                pid     = fork();

                if(0 == pid){ /*child process*/
                        setpgid(0,0);
                }else if(1 == pid){ /*parent process*/

                }else if(-1 == pid){ /*error*/
                        perror("fork");
                }

}


/*************************MAIN***************************/
int 
main (int argc, char **argv)
{

  FILE * inFile;
        FILE * outFile;
        pid_t pid;
  char * cmdLine;
  parseInfo *info; 
  /*info stores all the information returned by parser.*/
  struct commandType *com; 
  /*com stores command name and Arg list for one command.*/
  int j = 0; /* for keeping track of commands */
        char* k = malloc(5); /* to hold which command to grab from history*/
        int a = 0; /* for keeping track of bg jobs */
        int b = 0; /* random uses */

#ifdef UNIX
  
    fprintf(stdout, "This is the UNIX version\n");
#endif

#ifdef WINDOWS
    fprintf(stdout, "This is the WINDOWS version\n");
#endif

  while(1){
    /*insert your code to print prompt here*/

#ifdef UNIX
    cmdLine = readline(buildPrompt());
    if (cmdLine == NULL) {
      fprintf(stderr, "Unable to read command\n");
      continue;
    }

#endif

                /* this is to get rid of any unnecessary leading white space.
                                it works until you enter more than one space in front,
                                then it aborts and core dumps, cant figure out the
                                problem. */
                while(isspace(cmdLine[0]) && cmdLine[0] != '\0'){
                        if(strlen(cmdLine) == 1){/*if the string only contains a space*/
                                cmdLine[0] = '\0'; /*space into a null terminator*/
                        }else if(strlen(cmdLine) > 1){
                                cmdLine++; /*move the command up one to get rid of a space*/
                        }
                }

    
                /******************!x and !-x************************/
                if(cmdLine[0] == '!'){
                        if(cmdLine[1] == '-'){ /* !-x */
                                i = 2; /* we'll start after the !- */
                                while(isdigit(cmdLine[i])){/*keeps looping as long as digit*/
                                        k[b] = cmdLine[i];
                                        b++;
                                        i++;
                                } 
                                b = 0;
                                printf("%s \n", cmdLine);
                        }else if(isdigit(cmdLine[1])){ /* !x */
                                i = 1; /* we'll start after the ! */
                                while(isdigit(cmdLine[i])){/*keeps looping as long as digit*/
                                        k[b] = cmdLine[i];
                                        b++;
                                        i++;
                                }
                                b = 0;
                                printf("%s \n", cmdLine);
                        }else{ /* if anything but a number is after ! */
                                printf("That is not a valid number!");
                        }

                        if(hist[atoi(k)]){
                                cmdLine = hist[atoi(k)];
                        }else{
                                printf("%s: event not found \n", cmdLine);
                        }
                }


                /**************History*******************/
                        /* blanks and any command that starts 
                                with ! shouldn't be included
                                but it sometimes acts weird and does anyway,
                                can't figure out the problem */
                if(cmdLine[0] != '\0' && cmdLine[0] != '!'){
                        j++; /* To keep track of which command we are on.
                        Normally you'd add one after using it but I
                        left it here so I'd start at 1 like Bash */ 
                                hist[j] = malloc(strlen((cmdLine)+1));
                                strncpy(hist[j], cmdLine, strlen(cmdLine)+1);
                }

    /*calls the parser*/
    info = parse(cmdLine);
    if (info == NULL){
      free(cmdLine);
      continue; 
    }  
  
                /*i commented this out because it was annoying me*/
    /*prints the info struct
    print_info(info);*/

    /*com contains the info. of the command before the first "|"*/
    com=&info->CommArray[0];
    if ((com == NULL)  || (com->command == NULL)) {
      free_info(info);
      free(cmdLine);
      continue;
    }
    /* com->command tells the command name of com*/
                /* all the functions for these commands are
                         are implemented above main to keep things
                         concise and easy to read */
    if (isBuiltInCommand(com->command) == EXIT){
                                        b=0; /*to hold if there is a background process*/
                        for(i=0; i<1024; i++){ /*to loop through the job array*/
                                if(jobs[i] != NULL){
                                        b=1; /* this means there is a background process*/
                                }
                        }
                        if(0 == b){ /*if there are no jobs*/
                                /* since we are exiting the entire shell
                                we should free everything before hand */
                                free_info(info);
                                free(cmdLine);
        exit(0);
                        }else if( 1 == b){ /*if there is a job*/
                                printf("Jobs are in the background, still want to exit? (y or n)\n");
                                if(getchar() == 'y'){/*if the user decides to exit anyway*/
                                        free_info(info);
                                        free(cmdLine);
                exit(1);
                                }
                        }
    }
    if (isBuiltInCommand(com->command) == CD){
                        if (com->VarNum == 2){
                                change_dir(com->VarList[1]);
                        }else if(com->VarNum > 2){
                                printf("Too many arguments.\n");
                        }
    }
    if (isBuiltInCommand(com->command) == HISTORY){
      getHistory();
    }
    if (isBuiltInCommand(com->command) == KILL){
                        kill(jobs[atoi(com->VarList[1])]);      
                        printf("%s terminated.\n", jobs[atoi(com->VarList[1])]);
                        jobs[atoi(com->VarList[1])] = NULL;

    }
    if (isBuiltInCommand(com->command) == HELP){
      help();
    }
    if (isBuiltInCommand(com->command) == JOBS){
                        getJobs();
    }
  
    /*insert your code here.*/

                /**********************Jobs***************/
                if(cmdLine[strlen(cmdLine)-1] == '&'){
                        a++;  
                        jobs[a] = malloc(strlen(cmdLine)+1);
                        strcpy(jobs[a], cmdLine);
                        jobs[a][strlen(cmdLine)-1] = '\0';
                        printf("This job will be put in the background.\n");

                        bgJob();
                }

                /*********************STDIN&STDOUT**************/
                /*      currently not working, not sure the problem */
                if(info->boolInfile || info->boolOutfile){ /*if < or > */

                        pid = fork(); /*create child process from parent process*/

                        if(0 == pid){ /*child process*/
                                if(info->boolInfile){ /*stdin redirection*/
                                        inFile = fopen(info->inFile, "r");
                                        dup2(fileno(inFile), fileno(stdin));
                                        fclose(inFile);
                                }
                                if(info->boolOutfile){ /*stdout redirection*/
                                        outFile = fopen(info->outFile, "w");
                                        dup2(fileno(outFile), fileno(stdout));
                                        fclose(outFile);
                                }
                                execvp(info->CommArray[0].command, info->CommArray[0].VarList);
                        }
                        wait(0);/*wait for the child*/
                }

    free_info(info);
    free(cmdLine);
  }/* while(1) */
}
