#include "filesys_manager.h"

int main(int argc, char** argv) {
    struct Args* args;
    args = parse_arguments(argc, argv);

    // Filesystem to be managed
    struct Filesystem *filesys; 
    //TODO: Read from a file the filesys info if exists

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

    //TODO: Write in a file the filesys info

    return 0;
}