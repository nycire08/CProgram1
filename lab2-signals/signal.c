/* hello_signal.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int alarm_on = 0;
int num_alarm = 0;

time_t start, stop;

void handler(int signum)
{ 
  printf("Hello World!\n");
  sleep(1);
  alarm_on = 1;
  num_alarm++;
}

void handleSigint(int signum){
  int time_total;
  stop = time(NULL);
  time_total = stop - start;
  printf("The number of alarms were %d\n", time_total);
  exit(1);
}

int main(int argc, char argv[])
{
  signal(SIGALRM, handler); 
  signal(SIGINT, handleSigint);
  start = time(NULL);
  while(1){ 
    alarm_on = 0;
    alarm(2);
    while(!alarm_on);
      printf("Turing was right!\n");
      alarm_on = 1;
  }
  return 0; 
}
