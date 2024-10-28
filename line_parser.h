#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants of commands
#define CREATE  0
#define WRITE   1
#define READ    2
#define DELETE  3
#define LIST    4
#define UNKNOWN 5

struct Command {
    int command;
    char* filename;
    int size;
    int offset;
    char* data;
};

struct Command* parse_line(char*);
int parse_command(char*);