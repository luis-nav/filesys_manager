#include "filesystem.h"

struct Filesystem* read_filesystem(FILE* file) {
    //TODO: Implement;
    return NULL;
}

void write_filesystem(struct Filesystem* filesys, char* filename) {
    //TODO: Implement
}

void init_filesystem(struct Filesystem* filesys, char* filesys_name) {
    FILE *filesys_file = fopen(filesys_name, "r");
    if (filesys_file) {
        filesys = read_filesystem(filesys_file);
    } else {
        filesys->number_of_files = 0;
    }
}

// void set_data(char var_name, char* buffer, int start_index, int end_index) {
//     for (int i = start_index; i < end_index; i++) {
//         buffer[i] = var_name;
//     }
// }

void create_file(struct Filesystem* filesys, char* filename, int size) {
    if (filesys->number_of_files == MAX_FILE_NUMBER) {
        printf("Error: Maximum number of files reached. Exiting the app...");
        exit(1);
    }
    if (size == 0) {
        printf("Error: File cannot be of length 0. Exiting the app...");
        exit(1);
    }
    if (size > MAX_FILE_SIZE) {
        printf("Error: File cannot be of length greater than %d. Exiting the app...", MAX_FILE_SIZE);
        exit(1);
    }

    filesys->files[filesys->number_of_files].name = filename;
    filesys->files[filesys->number_of_files].size = size;
    
    struct BlockNode* past_node = NULL;
    // for (i < ceil(size/BLOCK_SIZE)):
    for (int i=0; i < ((size+(BLOCK_SIZE-1))/BLOCK_SIZE); i++) {
        int block_size = i+1 == ((size+(BLOCK_SIZE-1))/BLOCK_SIZE) ? BLOCK_SIZE : size%512;
        add_block(filesys, past_node, block_size);
    }
    filesys->number_of_files++;
}

// void add_block(struct MemoryTable* table, char var_name, int start_index, int end_index, int free_block_index) {
//     // New block
//     struct Block block = {var_name, false, start_index, end_index};
//     // Change free block and data 
//     table->blocks[free_block_index].start_index = block.end_index;
//     set_data(var_name, table->buffer, start_index, end_index);
    
    
//     // If free block was not fully occupied, create room for one more block:
//     if (table->blocks[free_block_index].start_index != table->blocks[free_block_index].end_index) {
//         table->size++;
//         // Move the rest of the blocks to the "right"
//         for (int i = table->size-1; i > free_block_index; i--) {
//             table->blocks[i] = table->blocks[i-1];
//         }
//     }
//     // Reassign free block to new block
//     table->blocks[free_block_index] = block;

// }

// void free_variable(struct MemoryTable* table, char var_name) {
//     // Set block as free 
//     for (size_t i = 0; i <= table->size; i++) {
//         if (table->blocks[i].var_name == var_name) {
//             table->blocks[i].free = true;
//             table->blocks[i].var_name = ' ';
//             set_data(table->blocks[i].var_name, table->buffer, table->blocks[i].start_index, table->blocks[i].end_index);
//         }
//     }

//     // Unite free blocks
//     bool past_block_free_flag = false;
//     int offset_counter = 0;
//     for (size_t i = 0; i < table->size; i++) {
//         // If there was an offset caused by the elimination of one block, the rest of the array should be reassigned
//         if (offset_counter && i < table->size) {
//             table->blocks[i] = table->blocks[i+offset_counter];
//         }

//         // If past and current blocks are both free, unite them
//         if (table->blocks[i].free && past_block_free_flag) {
//             table->blocks[i-1].end_index = table->blocks[i].end_index; 
//             offset_counter++;
//             table->size--;
//             i--;
//         }

//         past_block_free_flag = table->blocks[i].free;
//     }
// }

// void alloc_variable_best(struct MemoryTable* table, char var_name, int size) {
//     int best_option_index = -1;
//     int difference = INT_MAX;
//     for (size_t i = 0; i < table->size; i++) {
//         if (!table->blocks[i].free) continue;
//         int current_diff = table->blocks[i].end_index - table->blocks[i].start_index;
//         if (current_diff >= size && current_diff < difference) {
//             difference = current_diff;
//             best_option_index = i;
//         }
//     }
    
//     if (best_option_index == -1) {
//         printf("ERROR: Could not allocate %c", var_name);
//         exit(2);
//     }

//     add_block(
//         table,
//         var_name, 
//         table->blocks[best_option_index].start_index, 
//         table->blocks[best_option_index].start_index + size,
//         best_option_index
//     );
// }

// void alloc_variable_first(struct MemoryTable* table, char var_name, int size) {
//     bool assigned_flag = false;
//     for (size_t i = 0; i < table->size; i++) {
//         if (!table->blocks[i].free) continue;
//         if ((table->blocks[i].end_index - table->blocks[i].start_index) >= size) {
//             add_block(
//                 table,
//                 var_name, 
//                 table->blocks[i].start_index, 
//                 table->blocks[i].start_index + size,
//                 i
//             );
//             assigned_flag = true;
//             break;
//         }
//     }
//     if (!assigned_flag) {
//         printf("ERROR: Could not allocate %c\n", var_name);
//         exit(2);
//     }
// }

// void alloc_variable_worst(struct MemoryTable* table, char var_name, int size) {
//     int worst_option_index = -1;
//     int difference = -1;
//     for (size_t i = 0; i < table->size; i++) {
//         if (!table->blocks[i].free) continue;
//         int current_diff = table->blocks[i].end_index - table->blocks[i].start_index;
//         if (current_diff >= size && current_diff > difference) {
//             difference = current_diff;
//             worst_option_index = i;
//         }
//     }
    
//     if (worst_option_index == -1) {
//         printf("ERROR: Could not allocate %c\n", var_name);
//         exit(2);
//     }

//     add_block(
//         table,
//         var_name, 
//         table->blocks[worst_option_index].start_index, 
//         table->blocks[worst_option_index].start_index + size,
//         worst_option_index
//     );
// }

// void alloc_variable(struct MemoryTable* table, char var_name, int size, int algorithm)  {
//     if (algorithm == BEST) {
//         alloc_variable_best(table, var_name, size);
//     } else if (algorithm == FIRST) {
//         alloc_variable_first(table, var_name, size);
//     } else if (algorithm == WORST) {
//         alloc_variable_worst(table, var_name, size);
//     }
// }

// void realloc_variable(struct MemoryTable* table, char var_name, int new_size, int algorithm) {
//     free_variable(table, var_name);
//     alloc_variable(table, var_name, new_size, algorithm);
// }

// void print_table(struct MemoryTable* table) {
//     printf("=== Memory Table ===\n");
//     for (size_t i = 0; i < table->size; i++) {

//         printf("| %c  | %*d | %*d |\n", 
//             table->blocks[i].var_name, 
//             4, table->blocks[i].start_index, 
//             4, table->blocks[i].end_index
//         );
//     }
//     printf("====================");
//     printf("\nMemory view (32x32):\n");
//     for (size_t i = 0; i < table->capacity; i++) {
//         if (i % 32 == 0) {
//             printf("\n%*ld: ", 3, i);
//         }
//         printf(" %c", table->buffer[i]);
//     }
//     printf("\n");
// }