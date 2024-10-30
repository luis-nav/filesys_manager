#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define SYSTEM_SIZE         50022400
#define MAX_FILE_SIZE       1000448   
#define MAX_FILE_NUMBER     50
#define MAX_BLOCKS_PER_FILE 1954
#define BLOCK_SIZE          512

struct BlockNode {
    char buffer[BLOCK_SIZE];
    int size;
    struct BlockNode* system_next;
    struct BlockNode* file_next;
};

struct File {
    char* name;
    int size; 
    struct BlockNode* first_block;
};

struct Filesystem {
    int number_of_files;
    struct File files[MAX_FILE_NUMBER];
    struct BlockNode* first_block;
};

// File save/restart
struct Filesystem* read_filesystem(FILE*);
void write_filesystem(struct Filesystem*, char*);
// Block Management
void add_block(struct Filesystem*, struct BlockNode*, int);
// API Filesystem Functions
void init_filesystem(struct Filesystem*, char*);
void create_file(struct Filesystem*, char*, int);
void write_file(struct Filesystem*, char*, int, char*);
void read_file(struct Filesystem*, char*, int, int);
void delete_file(struct Filesystem*, char*);
void list_files(struct Filesystem*);