#include "filesys_manager.h"

int main(int argc, char** argv) {
    struct Args* args;
    args = parse_arguments(argc, argv);

    // Filesystem to be managed
    struct Filesystem *filesys; 
    init_filesystem(filesys, args->filesys_file_path);

    char line[256];
    while (fgets(line, sizeof(line), args->file) != NULL) {
        struct Command* cmd = parse_line(line);
        if (cmd != NULL) {
            if (cmd->command == CREATE) {
                create_file(&filesys, cmd->filename, cmd->size);
            } else if (cmd->command == WRITE) {
                write_file(&filesys, cmd->filename, cmd->offset, cmd->data);
            } else if (cmd->command == READ) {
                read_file(&filesys, cmd->filename, cmd->offset, cmd->size);
            } else if (cmd->command == DELETE) {
                delete_file(&filesys, cmd->filename);
            } else if (cmd->command == LIST) {
                list_files(&filesys);
            } else {
                printf("Could not read command... Exiting app...");
                exit(1);
            }
        }
    }

    write_filesystem(filesys, args->filesys_file_path);
    return 0;
}