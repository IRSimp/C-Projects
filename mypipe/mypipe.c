/*****************************************************************************
*The two commands should be entered by the user as arguments enclosed by " " and separated by |, e.g. ./mypipe "command1 | command2"
*If no arguments are entered by the user, the program will assume command 1 is ls -l and command 2 is sort.
*The correctness of both commands is totally at the discretion of the user                           *
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>               /* strsep, etc. */

#define MAX_NUM_ARGS 20           /* Maximum number of arguments allowed */
#define MAX_STR_LEN 200           /* Maximum string length */



int main(int argc, char * argv[])
{
       int fd[2];                 /* Two ends of the pipe */
       char * lhs = NULL;         /* Left hand side command of the pipe */
       char * rhs = NULL;          /* Right hand side command of the pipe */
       char * lhscommand = "ls";  /* Default command name on left hand side of pipe */
       char * rhscommand = "sort"; /* Default command name on right hand side of pipe */
       char * lhsargs[MAX_NUM_ARGS] = { "ls", "-l", NULL };   /* Default LHS args */
       char * rhsargs[MAX_NUM_ARGS] = { "sort", NULL };       /* Default RHS args */

    
       /*Parse the user input to extract the commands and their arguments*/
       /*Hint: read about strsep(3) */
       if(argc > 2){
		printf("Usage:\n./mypipe['<LHS-command>|<RHS-command>']\n");
	}
	else if(argc == 2){
		if(strchr(argv[1], '|')){
		  lhs = strsep(&argv[1], "|");
		  rhs = strsep(&argv[1], "|");
		}else printf("Usage:\n./mypipe['<LHS-command>|<RHS-command>']\n");
		int i = 0;
		char *str;
		while( (str = strsep(&lhs, " ")) != NULL){
			if(*str == '\0') continue; //skip spaces
			if(i == 0){
		  	lhscommand = str;  //override default
			}
			lhsargs[i++] = str; //build array of args starting with command
		}
		lhsargs[i] = NULL; //end args with null for exec
		i = 0;
		while( (str = strsep(&rhs, " ")) != NULL){
			if(*str == '\0') continue;
			if(i == 0){
        	          rhscommand = str;
        	        }
        	        rhsargs[i++] = str;
        	}
		rhsargs[i] = NULL;
	}

       /* Create the pipe */
       pipe(fd);     /* fd[0] is read end, fd[1] is write end */


       /* Do the forking */
       switch ( fork() )
       {
		case -1:
		  perror("fork bad");
		  exit(1);
               /* The LHS of command 'ls -l|sort' i.e. 'ls' should be
               executed in the child. */
		case 0:
		  close(fd[0]); //close read
		  dup2(fd[1], STDOUT_FILENO); //dup2 is used to change read end to what would come out of first command to pipe to the right end
		  execvp(lhscommand, lhsargs);
               /*The RHS of command 'ls -l|sort' i.e. 'sort' should be
                executed in the parent. */
		default:
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO); //same as above but opposite
		execvp(rhscommand, rhsargs);
       }

}
