

/*

Aiden Simpson
CSCI 347
10/17/2025

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "builtin.h"

/* Constants */ 

#define LINELEN 1024

/* Prototypes */

void processline (char *line);
char ** arg_parse(char *line, int *argcptr);

/* Shell main */

int
main (void)
{
    char   buffer [LINELEN];
    int    len;

    while (1) {
        /* prompt and get line */
	fprintf (stderr, "%% ");
	if (fgets (buffer, LINELEN, stdin) != buffer)
	  break;

        /* Get rid of \n at end of buffer. */
	len = strlen(buffer);
	if (buffer[len-1] == '\n')
	    buffer[len-1] = 0;

	/* Run it ... */
	processline (buffer);

    }

    if (!feof(stdin))
        perror ("read");

    return 0;		/* Also known as exit (0); */
}


void processline (char *line)
{
    pid_t  cpid;
    int    status;
    int    use;
    char expansion[LINELEN];

    if (expand(line, expansion, LINELEN) != 1) {
	fprintf(stderr, "error: line did not expand correctly\n");
	return;
    }

    char **argv = arg_parse(expansion, &use);

    if(is_func(argv)) {
	exec_func(argv, use);
	free(argv);
	return;
    }

    /* Start a new process to do the job. */
    cpid = fork();
    if (cpid < 0) {
      /* Fork wasn't successful */
      perror ("fork");
      return;
    }
    
    /* Check for who we are! */
    if (cpid == 0) {
      /* We are the child! */
      execvp (argv[0], argv);
      /* execlp reurned, wasn't successful */
      perror ("exec");
      fclose(stdin);  // avoid a linux stdio bug
      exit (127);
    }
    
    /* Have the parent wait for child to complete */
    if (wait (&status) < 0) {
      /* Wait wasn't successful */
      perror ("wait");
    }
}

char ** arg_parse(char *line, int *argcptr){

 /*point allows us to search through the input to find the spaces and count the args*/
    int quotebool = 0;
    char *point = line;
    int argcount = 0;
/*while we iterate the first time only counting the args is important so we can allocate the space to put the args*/
    while(*point){

//skip all spaces
        while(*point == ' ') {
            point++;
        }

//end of line
	if(*point == '\0') break;

	argcount++;
//count the arg before iterating trying to find the next space and also skip quotes if a quote is found change quotebool to 1
	quotebool = 0;
        while(*point){
            if(*point == '"'){
		quotebool = !quotebool;
		point++;

	    } else if (quotebool == 0 && *point == ' '){//end args at spaces outside quotes
	        break;
	    }else{
	        point++;
	    }
        }

    }
//malloc the size of our array plus 1 for the null at the end
    char **args = malloc(sizeof(char *) * (argcount + 1));

    int index = 0;
    point = line;
//whiles to do the same thing as before but add the args to the malloc array
    while(*point){

        while(*point == ' ') {
            point++;
        }

        if (*point == '\0') break;
//adds the args to the array args
//strbeg tracks the beginning of a string to be used later so we can find the end of the quotes
	char *write = point;
	char *strbeg = point;
	quotebool = 0;
	while (*point){
	    if(*point == '"'){
		quotebool = !quotebool;
		point++;
	    } else if(quotebool == 0 && *point == ' '){
		point++;
		break;
	    } else {
		*write = *point;//update write and point at the end of args
		write++;
		point++;
	    }
	}
	*write++ = '\0';
	args[index] = strbeg; //string beginning is stored to the end of current arg in index position
	index++;
	while(*point == ' ')point++;
    }


    args[index] = NULL;//end the array with null to pass to execvp
    *argcptr = argcount; //save the arg count for reference
    return args;

}
