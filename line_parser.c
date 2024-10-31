#include "line_parser.h"

// Delimiters
const char *delimiters = " \n";

int parse_command(char *token) {
    if (strcmp(token, "CREATE") == 0)
        return CREATE;
    else if (strcmp(token, "WRITE") == 0)
        return WRITE;
    else if (strcmp(token, "READ") == 0)
        return READ;
    else if (strcmp(token, "DELETE") == 0)
        return DELETE;
    else if (strcmp(token, "LIST") == 0)
        return LIST;
    else
        return UNKNOWN;
}

struct Command* parse_line(char* line) {
    // Ignore comments
    if (!strncmp(line, "#", 1)) return NULL;

    char* token = strtok(line, delimiters);
    
    // Ignore blank lines
    if (token == NULL) {
        return NULL;
    }

    struct Command* cmd;
    cmd = malloc(sizeof(struct Command));
    
    // Define command type
    cmd->command = parse_command(token);

    // If it is not LIST or UNKNOWN, it must have a filename
    if (cmd->command < 4) {
        token = strtok(NULL, delimiters);
        if (token != NULL) {
            cmd->filename = token;
        } else {
            printf("Expected filename after CREATE, WRITE, READ OR DELETE\nLine: %s", line);
            exit(1);
        }
    }

    // WRITE and READ cases (both have offset)
    if (cmd->command == WRITE || cmd->command == READ) {
        token = strtok(NULL, delimiters);
        if (token != NULL) {
            cmd->offset = atoi(token);
        } else {
            printf("Expected offset after WRITE OR READ\nLine: %s", line);
            exit(1);
        }

        // Get WRITE data
        if (cmd->command == WRITE) {
            token = strtok(NULL, "\n");
            if (token != NULL) {
                cmd->data = token;
            } else {
                printf("Expected data after offset on WRITE\nLine: %s", line);
                exit(1);
            }
        }
        // Get READ size
        if (cmd->command == READ) {
            token = strtok(NULL, delimiters);
            if (token != NULL) {
                cmd->size = atoi(token);
            } else {
                printf("Expected size after offset on READ\nLine: %s", line);
                exit(1);
            }
        }
    }
    // Get CREATE size
    if (cmd->command == CREATE) {
        token = strtok(NULL, delimiters);
        if (token != NULL) {
            cmd->size = atoi(token);
        } else {
            printf("Expected size after filename on CREATE\nLine: %s", line);
            exit(1);
        }
    }
    return cmd;
}