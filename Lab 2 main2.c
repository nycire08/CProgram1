#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>

#define   MAX_COUNT  1

void  ChildProcess(void);                /* child process prototype  */
void  ParentProcess(void);               /* parent process prototype */

void  main(void)
{
     int pid_t, pid_c1;

     pid_t = fork();

     if (pid_t == 0) {
          ChildProcess(); }
     else {
          ParentProcess();
     };
     
    
    pid_c1 = fork();

    if (pid_c1 == 0) {
          ChildProcess(); }
     else {
          ParentProcess();
     } ;




}


void  ChildProcess(void)
{
     int i;


     for (i = 0; i <= MAX_COUNT; i++) {
          printf("Child Pid:, %d is going to sleep!\n", getpid());
          int number;
          number = (rand() % (10 - 1 + 1)) + 1;
          sleep(number);
          //  
          printf("Child Pid:,%d is awake!\nWhere is my Parent: %d\n", getpid(), getppid());

          };
          printf("***Child process is done ***\n");
          exit(0);
     };
     

void  ParentProcess(void)
{
     int   i;
     int status, child_pid;

     for (i = 1; i <= MAX_COUNT; i++){
        child_pid = wait(&status);
          printf("Child Pid: %d has completed \n", child_pid);
     }
    printf("*** Parent is done ***\n");
};
