#include "arg_parser.h"

struct Args* parse_arguments(int argc, char** argv) {
    if (argc != 3) {
        printf("Unable to start: Usage: filesys_manager [FILESYS_FILE_PATH] [SCRIPT]\n");
        exit(1);
    }

    char* filesys_file_path = argv[1];
    FILE *script = fopen(argv[2], "r");

    if (!script) {
        printf("The given script file does not exist\n");
        exit(1);
    }

    struct Args* args;
    args = malloc(sizeof(struct Args));
    args->filesys_file_path = filesys_file_path;
    args->file = script;

    return args;
}