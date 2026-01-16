#ifndef BUILTIN_H
#define BUILTIN_H

int expand(char *orig, char *new, int newsize);

int is_func(char **argv);

int exec_func(char **argv, int argc);

#endif
