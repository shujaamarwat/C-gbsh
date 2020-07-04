#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>

#define RESET "\033[0m"
#define BOLDCYAN "\033[1m\033[36m"
#define BOLDYELLOW "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDWHITE "\033[1m\033[37m"

// char enlist[100][100];
// char valist[100][100];
// int numen = 0;

static pid_t GBSH_PID;
static pid_t GBSH_PGID;
static int GBSH_IS_INTERACTIVE;
static struct termios GBSH_TMODES;

static char* cwDir;
extern char** environ;

struct sigaction act_child;
struct sigaction act_int;

int chck_pr;

pid_t pid;

void signalHandler_child(int p);
void signalHandler_int(int p);
int changeDir(char * args[]);

// void prEnv(){
//     for (int i =0; i<numen; i++)
//     {
//         printf("\n%s=%s",enlist[i],valist[i]);
//     }
// }


void init(){
        GBSH_PID = getpid();
        GBSH_IS_INTERACTIVE = isatty(STDIN_FILENO);  

		if (GBSH_IS_INTERACTIVE) {
			while (tcgetpgrp(STDIN_FILENO) != (GBSH_PGID = getpgrp())){
					kill(GBSH_PID, SIGTTIN);
            }
			act_child.sa_handler = signalHandler_child;
			act_int.sa_handler = signalHandler_int;			
			
			// struct sigaction {
			// 	void (*sa_handler)(int);
			// 	void (*sa_sigaction)(int, siginfo_t *, void *);
			// 	sigset_t sa_mask;
			// 	int sa_flags;
			// 	void (*sa_restorer)(void);
				
			// }
			
			sigaction(SIGCHLD, &act_child, 0);
			sigaction(SIGINT, &act_int, 0);
			
			setpgid(GBSH_PID, GBSH_PID);
			GBSH_PGID = getpgrp();
			if (GBSH_PID != GBSH_PGID)
            {
					printf("Error, the shell is not process group leader");
					exit(EXIT_FAILURE);
			}
			tcsetpgrp(STDIN_FILENO, GBSH_PGID);  
			tcgetattr(STDIN_FILENO, &GBSH_TMODES);
			cwDir = (char*) calloc(1024, sizeof(char));
        }
        else
        {
                printf("Could not make the shell interactive.\n");
                exit(EXIT_FAILURE);
        }
}

// int userInput(char* str) 
// { 
//     char buf[1000]; 
  
//     // buf = readline("\n>>> "); 
//     char * tokens[256];
//     // scanf("%s",buf);
//     //fgets(buf,1000,stdin);
//     memset (buf, '\0', 1000);

//     scanf ("%[^\n]%*c", buf);
//     // printf("HI3\n");
//     if (strlen(buf) != 0 || (tokens[0] = strtok(buf,"\n")) != NULL) {

//     //  add_history(buf); 
//         // strcpy(str, buf);
//         CmdHan(buf);
//         return 0; 
//     }
//     else
//     {
//         return 1;
//     } 
// }

void signalHandler_child(int p){
	while (waitpid(-1, NULL, WNOHANG) > 0) {}
	printf("\n");
}

void signalHandler_int(int p){
	if (kill(pid,SIGTERM) == 0)
    {
		printf("\nProcess %d received a SIGINT signal\n",pid);
		chck_pr= 1;			
	}
    else
    {
		printf("\n");
	}
}

// int remEnv(char* x)
// {
//     if (strcmp(enlist[numen-1],x)!=0)
//     {
//         int i;
//         char* prev = enlist[numen-1];
//         char* Vaprev = valist[numen-1];
//         for (i=numen-1; i>=0 && enlist[i]!=x; i--)
//         {
//             char* curr = enlist[i];
//             strcpy(enlist[i],prev);
//             // enlist[i] = prev;
//             prev = curr;
//             char* Vacurr = valist[i];
//             strcpy(valist[i],Vaprev);
// //            valist[i] = Vaprev;
//             Vaprev = Vacurr;
//         }
//         unsetenv(enlist[i]);
//         if (i >= 0)
//         {
//             strcpy(enlist[i],prev);
//             strcpy(valist[i],Vaprev);

//             // enlist[i] = prev;
//             // valist[i] = Vaprev;
//         }
//         return 1;
//     }
//     return 0;
// }

void prompt_shell(){
    //    clear();
//    system("clear");

	char hostn[1204] = "";
    //GET USERNAME
    //    char* username = getenv("USER");

    //GET HOSTNAME
	gethostname(hostn, sizeof(hostn));

    //GET CURRENT WORKING DIRECTORY
    // char cwd[1000];
    // getcwd(cwd, sizeof(cwd));

	printf(BOLDYELLOW"%s@%s"RESET BOLDWHITE"%s > "RESET, getenv("LOGNAME"), hostn, getcwd(cwDir, 1024));
    //    printf("\n");
//    sleep(1);
//    clear();
}

int changeDir(char* args[]){
	if (args[1] == NULL) {
		chdir(getenv("HOME")); 
		return 1;
	}
	else
    {
		if (chdir(args[1]) == -1)
        {
			printf(" %s: No such directory exists\n", args[1]);
            return -1;
		}
	}
	return 0;
}

int EnvMng(char * args[], int option){
	char **env_aux;

    //  int i;
    //  char* prev = enlist[option-1];
    //  char* Vaprev = valist[option-1];

	switch(option)
    {
		case 0: 
			for(env_aux = environ; *env_aux != 0; env_aux ++)
            {
				printf("%s\n", *env_aux);
			}
			break;
		case 1: 
			if((args[1] == NULL) && args[2] == NULL){
				printf("%s","Not enought input arguments\n");
				return -1;
			}
			if(getenv(args[1]) != NULL)
            {
				printf("%s", "The variable has been overwritten\n");
			}
            else
            {
				printf("%s", "The variable has been created\n");
			}
			
			if (args[2] == NULL)
            {
				setenv(args[1], "", 1);
			}
            else
            {
				setenv(args[1], args[2], 1);
			}
			break;
		case 2:
			if(args[1] == NULL)
            {
				printf("%s","Not enough input arguments\n");
				return -1;
			}
			if(getenv(args[1]) != NULL)
            {
				unsetenv(args[1]);
				printf("%s", "The variable has been erased\n");
			}
            else
            {
				printf("%s", "The variable does not exist\n");
			}
		break;
	}
	return 0;
}
 
void launchProg(char **args, int bg){	 
	 int err = -1;
	 
	 if((pid=fork())==-1)
     {
		 printf("Child process could not be created\n");
		 return;
	 }
	if(pid==0){
		signal(SIGINT, SIG_IGN);

		setenv("parent",getcwd(cwDir, 1024),1);	
		if (execvp(args[0],args)==err)
        {
			printf("Command not found");
			kill(getpid(),SIGTERM);
		}
	 }
	 if (bg == 0)
     {
		 waitpid(pid,NULL,0);
	 }
     else
     {
		 printf("Process created with PID: %d\n",pid);
	 }	 
}


void fileIO(char * args[], char* inputFile, char* outputFile, int option){
	 
	int err = -1;
	
	int fileDesc;
	
	if((pid=fork())==-1)
    {
		printf("Child process could not be created\n");
		return;
	}
	if(pid==0)
    {
		if (option == 0){
			fileDesc = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600); 
			dup2(fileDesc, STDOUT_FILENO); 
			close(fileDesc);
		}else if (option == 1){
			fileDesc = open(inputFile, O_RDONLY, 0600);  
			dup2(fileDesc, STDIN_FILENO);
			close(fileDesc);
			fileDesc = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
			dup2(fileDesc, STDOUT_FILENO);
			close(fileDesc);		 
		}
		 
		setenv("parent",getcwd(cwDir, 1024),1);
		
		if (execvp(args[0],args)==err){
			printf("err");
			kill(getpid(),SIGTERM);
		}		 
	}
	waitpid(pid,NULL,0);
}

void pipHand(char * args[]){
	int filedes[2];
	int filedes2[2];
	
	int num_cmds = 0;
	
	char *command[256];
	
    //pid_t pid = fork();  

	pid_t pid;

	int err = -1;
	int end = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;

//     if (pid == -1) {
//         printf("\nFailed forking child..");
//         return;
//     }
//     else if (pid == 0) {
//         if (execvp(parsed[0], parsed) < 0)
//         {
//             printf("\nCould not execute command");
//         }
//         exit(0); 
//     } 
//     else {
//         wait(NULL);  
//         return; 
//     } 

	while (args[l] != NULL)
    {
		if (strcmp(args[l],"|") == 0){
			num_cmds++;
		}
		l++;
	}
	num_cmds++;
	
	while (args[j] != NULL && end != 1)
    {
		k = 0;
		while (strcmp(args[j],"|") != 0)
        {
			command[k] = args[j];
			j++;	
			if (args[j] == NULL)
            {
				end = 1;
				k++;
				break;
			}
			k++;
		}
		command[k] = NULL;
		j++;		
		
		if (i % 2 != 0)
        {
			pipe(filedes);
		}
        else
        {
			pipe(filedes2);
		}
		
		pid=fork();
		
		if(pid==-1)
        {			
			if (i != num_cmds - 1)
            {
				if (i % 2 != 0)
                {
					close(filedes[1]);
				}
                else
                {
					close(filedes2[1]);
				} 
			}
			printf("Child Process NOT created\n");
			return;
		}
		if(pid==0)
        {

			if (i == 0)
            {
                //dup2(filedes[1],1);

				dup2(filedes2[1], STDOUT_FILENO);
			}
			else if (i == num_cmds - 1)
            {
				if (num_cmds % 2 != 0)
                {
					dup2(filedes[0],STDIN_FILENO);
                    //dup2(filedes[0],1);
				}
                else
                {
                    //dup2(filedes[0],0);
					dup2(filedes2[0],STDIN_FILENO);
				}
			}
            else
            {
				if (i % 2 != 0){
                    //dup2(filedes[0],0);
					dup2(filedes2[0],STDIN_FILENO);
					dup2(filedes[1],STDOUT_FILENO);
				}
                else
                {
					dup2(filedes[0],STDIN_FILENO); 
					dup2(filedes2[1],STDOUT_FILENO);					
				} 
			}
			
			if (execvp(command[0],command)==err){
				kill(getpid(),SIGTERM);
			}		
		}
				
		if (i == 0)
        {
            //close(filedes[1]);
			close(filedes2[1]);
		}
		else if (i == num_cmds - 1)
        {
			if (num_cmds % 2 != 0)
            {
				close(filedes[0]);
			}
            else
            {
				close(filedes2[0]);
			}
		}
        else
        {
			if (i % 2 != 0)
            {					
				close(filedes2[0]);
				close(filedes[1]);
			}
            else{					
				close(filedes[0]);
				close(filedes2[1]);
			}
		}
				
		waitpid(pid,NULL,0);
				
		i++;	
	}
}
			
int CmdHand(char * args[]){
	int i = 0;
	int j = 0;
	// int exCmd= 0;
    // char* ComL[4];
    // char cwd[100];
    // char * token;
	int fileDesc;
	int standardOut;

    // char envar[100];
    // char value[100];
    // int coms =4;
	int aux;
	int bg = 0;	
    // int f=0;
    // char dr[1000];

	char *args_aux[256];
	
    // getcwd(cwd, sizeof(cwd));

    // ComL[0] = "exit";
    // ComL[1] = "pwd";
    // ComL[2] = "clear";
    // ComL[3] = "environ";

	while ( args[j] != NULL)
    {
		if ((strcmp(args[j],">") == 0) || (strcmp(args[j],"<") == 0) || (strcmp(args[j],"&") == 0))
        {
            // exCmd= i + 1;
			break;
		}
		args_aux[j] = args[j];
		j++;
	}

	////    switch (exCmd) {
    // if(exCmd==1){
    //     printf("\nexit successfull\n");
    //     exit(0);
    // }
    // if(exCmd==2){
    //     printf("\nCurrent Directory:  %s", cwd);
    //     return 1;
    // } 
    // if(exCmd==3){
    //     system("clear");
    //     return 1;
    // }
    // if(exCmd==4){
    //     // printf("=\n");
    //     prEnv();        
    //     return 1;
    // }

	if(strcmp(args[0],"exit") == 0)
    {
        exit(0);
    }

 	else if (strcmp(args[0],"pwd") == 0){
		if (args[j] != NULL)
        {
			if ( (strcmp(args[j],">") == 0) && (args[j+1] != NULL) )
            {
				fileDesc = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600); 
				standardOut = dup(STDOUT_FILENO);
				dup2(fileDesc, STDOUT_FILENO); 
				close(fileDesc);
				printf("%s\n", getcwd(cwDir, 1024));
				dup2(standardOut, STDOUT_FILENO);
			}
		}
        else
        {
			printf("%s\n", getcwd(cwDir, 1024));
		}
	} 
	else if (strcmp(args[0],"clear") == 0)
    {
        system("clear");
    }
	else if (strcmp(args[0],"cd") == 0)
    {
        changeDir(args);
    }
	else if (strcmp(args[0],"environ") == 0)
    {
        // printf("=\n");
		if (args[j] != NULL)
        {
        //     //         int fo = 0;
        // token = strtok(NULL, " ");
        // // printf(" %s\n", token);
        // strcpy(envar,token);
        // // envar = token;
        // //printf("%s\n",envar);

        // token = strtok(NULL, " ");
        // // printf(" %s\n", token);
        // strcpy(value,token);
        // //value = token;
        // //printf("%s\n",value);
		if ((strcmp(args[j],">") == 0) && (args[j+1] != NULL))
            {
				fileDesc = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
				standardOut = dup(STDOUT_FILENO);
				dup2(fileDesc, STDOUT_FILENO); 
				close(fileDesc);
				EnvMng(args,0);
				dup2(standardOut, STDOUT_FILENO);
			}
		}
        else
        {
			EnvMng(args,0);
		}
	}
	else if (strcmp(args[0],"setenv") == 0)
    {
        EnvMng(args,1);

//         for (int i =0; i<numen; i++){
//             if (strcmp(enlist[i], envar)==0){
//                 printf("Environment Already Defined \n");
//                 fo=1;
//                 break;
//             }
//         }
//         if(fo==0){
// //            printf("numen =%d\n",numen);
//             strcpy(enlist[numen],envar);
//             //enlist[numen]=envar;
//             // printf(".%s.\n",enlist[numen]);
//             strcpy(valist[numen],value);
//             //valist[numen]=value;
//             setenv(enlist[numen],valist[numen],1);
//             printf("Environment Defined\n");
//             numen++;
//         }

    }
	else if (strcmp(args[0],"unsetenv") == 0)
    {
        // token = strtok(NULL, " ");
        // // printf(" %s\n", token);
        // //envar = token;
        // strcpy(envar,token);
        // int foRem = remEnv(envar);
        // if(foRem ==1){
        //     numen--;
        //     printf("Environment Undefined\n");
        // }
        // else{
        //     printf("Couldnt Remove, Environment Not Found\n");
        // }
        
        EnvMng(args,2);
    }
	else
    {
        // strcpy(dr,inp);
        // token = strtok(inp, " ");

        // if (strcmp(token, "ls") == 0){
        //     token = strtok(NULL, " ");
        //     // break;
        //     // f=1;
        //     struct dirent *dp;
        //     DIR *dir = opendir(token);
        //     if (!dir){
        //         printf("No Such Directory Exists or command failed\n");
        //     }
        //     else{
        //         while ((dp = readdir(dir)) != NULL)
        //         {
        //             printf(BOLDBLUE "%s\n" RESET , dp->d_name);
        //         }
        //     }
        //     closedir(dir);
        // else if (strcmp(token, "cd") == 0)
        // {
        //     // token = strtok(NULL, " ");
        //     // printf( "2. %s,\n",token);
        //     // while( token != NULL) {
        //     //     // printf( "%s\n",token);
        //     //     //printf( "%s ..... %d\n",token,i);
        //     //     dr[i]=token;
        //     //     i++;
        //     //     token = strtok(NULL, " ");
        //     // }
        //     token = strtok(NULL, " ");
        //     printf(" %s\n", token);

        //     // char st[1000],p;
        //     //printf(" %ld\n", strlen(dr));
        //     // strcpy(st, inp);
        //     // printf(" %s\n", st);
        //     if (chdir(token) != 0){
        //         printf("No Such Directory Exists or command failed\n");
        //     }
        //     else{
        //         printf("DIRECTORY CHANGED To: %s\n",token);
        //     }
        //     // break;
        //     // f=1;
        // }

		while (args[i] != NULL && bg == 0){
			if (strcmp(args[i],"&") == 0){
				bg = 1;
			}
            else if (strcmp(args[i],"|") == 0)
            {
				pipHand(args);
				return 1;
			}
            else if (strcmp(args[i],"<") == 0)
            {
				aux = i+1;
				if (args[aux] == NULL || args[aux+1] == NULL || args[aux+2] == NULL)
                {
					printf("Not enough input arguments\n");
					return -1;
				}
                else
                {
					if (strcmp(args[aux+1],">") != 0){
						printf("Expected '>' and found %s\n",args[aux+1]);
						return -2;
					}
				}
				fileIO(args_aux,args[i+1],args[i+3],1);
				return 1;
			}
			else if (strcmp(args[i],">") == 0)
            {
				if (args[i+1] == NULL)
                {
					printf("Not enough input arguments\n");
					return -1;
				}
				fileIO(args_aux,NULL,args[i+1],0);
				return 1;
			}
			i++;
		}

		args_aux[i] = NULL;
		launchProg(args_aux,bg);
		
		//	i = 0;
		//	while(args[i]!=NULL){
		//		printf("%s\n", args[i]);
		//		i++;
		//	}
	}
    return 1;
}

int main(int argc, char *argv[], char ** envp) {
	char line[1000];
	char * tokens[256];
	int numTokens;
	chck_pr= 0;
	pid = -10;

    // int execFlag = 0;
    // char* pAP[100];
    // char*pA[100];
    // int res;
    system("clear");

	init();
	environ = envp;
	// fprintf(stdout, "Hello World!\n");

	setenv("shell",getcwd(cwDir, 1024),1);
	
	do
    {
    	// shell code here
	
	    // ...
	
	    // ...

		if (chck_pr== 0)
        {
            prompt_shell();
        }
		chck_pr= 0;
		memset ( line, '\0', 1000);
		fgets(line, 1000, stdin);
	
		if((tokens[0] = strtok(line," \n\t")) == NULL)
        {
            continue;
        }
		numTokens = 1;
        
        // execFlag = processString(inp, pA, pAP);
        // if (execFlag == 1){ 
        //     execArgs(pA);
        // }

		while((tokens[numTokens] = strtok(NULL, " \n\t")) != NULL)
        {
            numTokens++;
        }
		CmdHand(tokens);
		
	}while(1);
	exit(0);
}