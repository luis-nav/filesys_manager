#include <stdio.h>
#include <stdlib.h>

struct Args {
    char* filesys_name;
    FILE* file;
};

struct Args* parse_arguments(int, char**);