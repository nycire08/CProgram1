#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>


//I received help on this Assignment (Deontae Smith)


#define MAX_COMMAND_LINE_LEN 1024


#define MAX_COMMAND_LINE_ARGS 128







char prompt[] = "> ";


char delimiters[] = " \t\r\n";


extern char **environ;


int cmd_pid = -1;







void exitcode(int code);


void executecommand(char* arguments[]);


void removeToken(char* token);


void timeout_process(int time, int pid);


void sig_handler(int signum);







int main() {
 int i;
 char command_line[MAX_COMMAND_LINE_LEN];
 char cmd_bak[MAX_COMMAND_LINE_LEN];
 size_t wd_size = 400;
 char wd[wd_size];
 char *wdp;

 char *arguments[MAX_COMMAND_LINE_ARGS];

 signal(SIGINT,sig_handler);

 while (true) {

   do{


       fflush(stdout);
       wdp = getcwd(wd, wd_size);
       printf("%s %s", wdp, prompt);
       fflush(stdout);
       if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
           fprintf(stderr, "fgets error");
           exit(0);
       }
 }while(command_line[0] == 0x0A);  // while just ENTER pressed

   if (feof(stdin)) {
     exitcode(0);


   }
 arguments[0] = strtok(command_line, delimiters);

   i=0;


   while(arguments[i] != NULL) {
     removeToken(arguments[i]);
     i++;
     arguments[i]= strtok(NULL, delimiters);

   }


   char* last_token = arguments[i-1];
   bool run_in_background = false;
   if (strcmp(last_token,"&")==0){
       run_in_background = true;
      arguments[i-1] = NULL;


   }

   if (strcmp(arguments[0],"cd")==0){
     chdir(arguments[1]);


   }


   else if (strcmp(arguments[0],"pwd")==0){


     printf("%s\n",getcwd(wd, wd_size));


   }


   else if (strcmp(arguments[0],"echo")==0){


     i = 1;


     while(arguments[i] != NULL){


       if (arguments[i][0] == '$'){


         printf("%s ", getenv(arguments[i]+1));


       }


       else{


         printf("%s ", arguments[i]);


       }


       i++;


     }


     printf("\n");


   }


   else if (strcmp(arguments[0],"exit")==0){


     exit(0);


   }


   else if (strcmp(arguments[0],"env")==0){


     if (arguments[1] != NULL){


       printf("%s\n",getenv(arguments[1]));


     }


     else{


       char** env= environ;


       for (; *env; env++)


       printf("%s\n", *env);


     }


   }


   else if (strcmp(arguments[0],"setenv")==0){


     char* temp[2];


     temp[0] = strtok(arguments[1], "=");


     i=0;


     while(temp[i] != NULL || i == 2) {


       i++;


       temp[i] = strtok(NULL, "=");


     }


     if (temp[0] == NULL || temp[1] == NULL){


       exitcode(1);


     }


     setenv(temp[0], temp[1], 1);


   }


   else{


     int pid = fork();


     if (pid == 0){


       signal(SIGINT, SIG_DFL);


       executecommand(arguments);


       exit(0);


     }


     else{


       if (run_in_background){


         cmd_pid = -1;


       }


       else {


         cmd_pid = pid;


         int kpid = fork();


         if (kpid==0){


           signal(SIGINT, SIG_DFL);


           timeout_process(10000, pid);


           exit(0);


         }


         else{


           waitpid(pid, NULL, 0);


           kill(kpid, SIGINT);


           waitpid(kpid, NULL, 0);


         }


       }


     }


    


   }







 }





 return -1;


}







void timeout_process(int time, int pid){


 sleep(time);


 printf("Foreground process timed out.\n");


 kill(pid, SIGINT);


}







void removeToken(char* token){


 bool quotes = false;


 char first;


 char last;


 if (token[0]=='\"' || token[0]=='\''){


   first = token[0];


   quotes = true;


 }


 if (quotes){


   int i=0;


   while (token[i]!='\0'){


     last = token[i];


     i++;


   }


   if (first == last){


     i=1;


     while (token[i]!='\0'){


       token[i-1] = token[i];


       i++;


     }


     token[i-1]='\0';


     token[i-2]='\0';


   }


 }


 


}







void executecommand(char* arguments[]){


 char* args_by_pipe[MAX_COMMAND_LINE_ARGS][MAX_COMMAND_LINE_ARGS];


 int i = 0;


 int command_count = 0;


 int command_token_count = 0;


 while (arguments[i]!= NULL){


   if (strcmp(arguments[i],"|")==0){


     if (command_token_count == 0){


       printf("Invalid pipe command\n");


       return;


     }


     args_by_pipe[command_count][command_token_count] = NULL;


    


     command_count++;


     command_token_count=0;


   }


   else{


     args_by_pipe[command_count][command_token_count] = arguments[i];


     command_token_count++;


   }


   i++;


 }






 int num_pipes = command_count;


 command_count++;


 if (num_pipes == 0){


   execvp(args_by_pipe[0][0],args_by_pipe[0]);


 }


 else{


   int pipes[num_pipes][2];


   for (i=0;i<num_pipes;i++){


     pipe(pipes[i]);


   }


   int pids[command_count];


   for (i=0;i<command_count;i++){


     pids[i]=fork();


     if (pids[i] == 0){


       int j;


       if (i == 0){


         dup2(pipes[0][1], 1);


         close(pipes[0][0]);


         for (j=1; j < num_pipes; j++){


           close(pipes[j][0]);


           close(pipes[j][1]);


         }


       }


       else if (i == num_pipes){


         dup2(pipes[num_pipes-1][0], 0);


         close(pipes[num_pipes-1][1]);


         for (j=0; j < num_pipes-1; j++){


           close(pipes[j][0]);


           close(pipes[j][1]);


         }


       }


       else{


         dup2(pipes[i-1][0],0);


         dup2(pipes[i][1],1);


         for (j=0; j < num_pipes; j++){


           if (j== i-1){


             close(pipes[j][1]);


           }


           else if (j == i){


             close(pipes[j][0]);


           }


           else{


             close(pipes[j][0]);


             close(pipes[j][1]);


           }


         }


       }


       execvp(args_by_pipe[i][0],args_by_pipe[i]);


     }


     else if (pids[i] < 0){


       printf("An error occured\n");


       return;


     }


   }


   for (i=0; i<num_pipes; i++){


     close(pipes[i][0]);


     close(pipes[i][1]);


   }


   for(i = 0; i < command_count; i++){


     wait(NULL);


   }


  


 }


}







void sig_handler(int signum)


{


 if (cmd_pid != -1){


   kill(cmd_pid, SIGINT);


 }


}







void exitcode(int code){


 printf("\n");


 fflush(stdout);


 fflush(stderr);


 exit(code);


}
