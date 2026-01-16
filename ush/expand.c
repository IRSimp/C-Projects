#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtin.h"

char* copystring(char *final, char *start, int n){

	int i;
	for(i = 0; i < n && start[i] != '\0'; i++) final[i] = start[i];
	for( ; i < n; i++) final[i] = '\0';
	return final;

}


int expand(char *orig, char *new, int newsize)
{

	int i = 0;
	int j = 0;

	while (orig[i] != '\0') {

		//start the expansion when we see this
		if (orig[i] == '$' && orig[i + 1] == '{') {
			i++;
			i++;
			int envar = i;

			while (orig[i] != '}' && orig[i] != '\0') i++;

			if(orig[i] == '\0'){
			fprintf(stderr, "missing closing bracket in environment variable\n");
			return 0;
			}

			int length = i - envar;
			if (length <= 0){
				fprintf(stderr, "missing environment variable in brackets -> ${}\n");
				return 0;
			}

			//to hold the varible to extract envvar
			char variable[128];
			if (length >= (int) sizeof(variable)){
				fprintf(stderr, "variable name too long must be less that 128 char\n");
				return -0;
			}
			//copy the env variable name to variable for getenv
			copystring(variable, &orig[envar], length);
			variable[length] = '\0';
			//getenv variable to put in place of the ${} and determine if it will fit in newsize
			char* env = getenv(variable);
			if(env != NULL){
				int explen = strlen(env);
				if(j + explen > newsize - 1 ) {
					fprintf(stderr, "expansion wont fit in new line buffer: overflow\n");
					return 0;
				}
				//copy what the env variable is in place of the ${NAME} if it will fit
				copystring(&new[j], env, newsize - j - 1);
				j += explen;
			}

			i++;

		} else {
			//advance while not in the section for expanding
			if(j >= newsize - 1) {
				fprintf(stderr, "output buffer is too small");
				return 0;
			}
			new[j++] = orig[i++];

		}

	}
	//end with the termination signal and return 1 so ush knows that we finished this successfully
	new[j] = '\0';
	return 1;
}

