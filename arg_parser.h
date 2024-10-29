#include <stdio.h>
#include <stdlib.h>

struct Args {
    char* filesys_file_path;
    FILE* file;
};

struct Args* parse_arguments(int, char**);