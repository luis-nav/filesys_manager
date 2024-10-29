#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define SYSTEM_SIZE         50022400
#define MAX_FILE_SIZE       1000448   
#define MAX_FILE_NUMBER     50
#define MAX_BLOCKS_PER_FILE 1954
#define BLOCK_SIZE          512

struct Block {
    bool free;
    int start_index;
    int end_index;
};

//TODO: File, File linked list or hashmap, implementations... 

struct Filesystem {
    char* buffer;
    size_t number_of_files;
    struct Block* blocks;
};

void init_filesystem(struct Filesystem*, char*);
void save_filesystem(struct Filesystem*, char*);
void set_block_data(char*, char*, int , int, int);
void create_file(struct Filesystem*, char*, int);
void write_file(struct Filesystem*, char*, int, char*);
void read_file(struct Filesystem*, char*, int, int);
void delete_file(struct Filesystem*, char*);
void list_files(struct Filesystem*);
// void add_block(struct MemoryTable*, char, int, int, int);
// void alloc_variable_first(struct MemoryTable*, char, int);
// void alloc_variable_worst(struct MemoryTable*, char, int);
// void alloc_variable(struct MemoryTable*, char, int, int);
// void realloc_variable(struct MemoryTable*, char, int, int);
// void free_variable(struct MemoryTable*, char);
// void print_table(struct MemoryTable*);