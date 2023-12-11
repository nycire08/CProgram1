#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>


 // recieved help from Yasmin 


int main()
{
   int pd1[2]; 
   int pd2[2]; 
   char fixed_str[] = "howard.edu";
   char input_str[100];
   char inputstr2[100];
   
   pid_t p;
  if (pipe(pd1)==-1)
   {
       fprintf(stderr, "Pipe Failed" );
       return 1;
   }

   if (pipe(pd2)==-1)

   {
       fprintf(stderr, "Pipe Failed" );
       return 1;
   }

   printf("Enter a string to concatenate:");
   scanf("%s", input_str);
   
   p = fork();
   if (p < 0)

   {
     fprintf(stderr, "fork Failed" );
       return 1;
   }

   else if (p > 0)
   {
     close(pd1[0]);  
     write(pd1[1], input_str, strlen(input_str)+1);

      wait(NULL);
      close(pd1[1]);
       close(pd2[1]); // close writing end of pipe 2

       char concat_str[100];
       read(pd2[0], concat_str, 100);

       printf("Concatenated string %s\n", concat_str);
       close(pd2[0]);
   }


   else
     {
       close(pd1[1]); 
       char concat_str[100];
       read(pd1[0], concat_str, 100);

       int k = strlen(concat_str);
       int i;
       for (i=0; i<strlen(fixed_str); i++)
      concat_str[k++] = fixed_str[i];
       concat_str[k] = '\0';   // string ends with '\0'

       printf("Concatenated string %s\n", concat_str);
       close(pd1[0]);


       printf("Enter a string to concatenate:");
       scanf("%s", inputstr2);
       strcat(concat_str, inputstr2);
       close(pd2[0]); 

       write(pd2[1], concat_str, strlen(concat_str)+1);
       wait(NULL);
       close(pd2[1]);

       exit(0);
   }
}
