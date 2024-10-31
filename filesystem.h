#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define SYSTEM_SIZE         50022400
#define MAX_FILE_SIZE       1000448   
#define MAX_FILE_NUMBER     50
#define MAX_BLOCKS_PER_FILE 1954
#define BLOCK_SIZE          512
#define FILENAME_SIZE       256

struct Index_Node {
    int index;
    struct Index_Node* next;
};

struct Block {
    char buffer[BLOCK_SIZE];
    bool free;
    int size;
};

struct File {
    char name[256];
    int file_size;
    int block_size; 
    struct Index_Node* first_block_index;
};

struct Filesystem {
    int number_of_files;
    struct File files[MAX_FILE_NUMBER];
    struct Block blocks[MAX_BLOCKS_PER_FILE*MAX_FILE_NUMBER];
};

// File save/restart
void read_filesystem(struct Filesystem*, FILE*);
void write_filesystem(struct Filesystem*, char*);
// Int linked list functions
struct Index_Node* create_node(int);
void append(struct File*, int);
void free_list(struct Index_Node*);
// API Filesystem Functions
void init_filesystem(struct Filesystem*, char*);
void create_file(struct Filesystem*, char*, int);
void write_file(struct Filesystem*, char*, int, char*);
void read_file(struct Filesystem*, char*, int, int);
void delete_file(struct Filesystem*, char*);
void list_files(struct Filesystem*);