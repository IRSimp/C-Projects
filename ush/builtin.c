#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtin.h"

static int shell_exit(char **argv, int argc);

static int set_env(char **argv, int argc);

static int unset_env(char **argv, int argc);

static int change_dir(char **argv, int argc);

//structure of function names and function pointers to call in a loop

typedef struct {
	const char *cmd;
	int (*func)(char **argv, int argc);
}command;

//cmd is the function name that shell will recognize and func is the function in this program

static command commands[] = {

	{"exit", shell_exit},
	{"envset", set_env},
	{"envunset", unset_env},
	{"cd", change_dir},
	{NULL, NULL}

};

//cd change directory to the location in argv[1] otherwise go home, if directory doesn't exist it will warn the user

static int change_dir(char **argv, int argc){
	char *dest = NULL;
	if(argc == 1){
		dest = getenv("HOME");
		if(!dest) {
			fprintf(stderr, "cd error no HOME set\n");
			return 0;
		}
	}else if(argc == 2){
		dest = argv[1];
	}else{ //you only get here if you have more than 2 args and it will tell the user the correct format
		fprintf(stderr, "error: try 'cd [directory]'\n");
		return 0;
	}
	int err = chdir(dest);
	if(err != 0){//error catch to check for directory
		fprintf(stderr, "No such directory\n");
		return 0;
	}
	return 1;
}

static int unset_env(char **argv, int argc){
	if(argc != 2){//anything other than 2 args will not work
		fprintf(stderr, "error try 'unset [env name]'\n");
		return 0;
	}
	int err = unsetenv(argv[1]); //unset argv[1]
	if(err != 0){
		fprintf(stderr, "unsetenv error\n");
		return 0;
	}
	return 1;
}

static int set_env(char **argv, int argc){
	if(argc != 3){//set needs 3 args for set name value
		fprintf(stderr, "error try 'envset [env name] [env value]'\n");
		return 0;
	}
	int err = setenv(argv[1], argv[2], 1);
	if(err != 0){
		fprintf(stderr, "setenv error\n");
		return 0;
	}
	return 1;
}

static int shell_exit(char **argv, int argc){
	if(argc > 2){//greater than 2 args prompts a try again
		fprintf(stderr, "error try 'exit [value]'\n");
		return 0;
	}
	if(argc == 2){
		int status = atoi(argv[1]);//exits with status argv[1]
		exit(status);
	}
	exit(0);//else exit with 0
	return 0;
}

int exec_func(char **argv, int argc){
	if(argv == NULL || argv[0] == NULL) return 0;//if argv is NULL return 0 and move on

	for(int i = 0; commands[i].cmd != NULL; i++){
		if(!strcmp(argv[0], commands[i].cmd)) return commands[i].func(argv,argc);//find the command that needs to be executed
	}
	return 0;
}

int is_func(char **argv){
	if(argv == NULL || argv[0] == NULL) return 0;

        for(int i = 0; commands[i].cmd != NULL; i++){
                if(!strcmp(argv[0], commands[i].cmd)) return 1;//same as above but we return 1 to pass the check in processline to avoid the fork
        }
        return 0;
}
