#include "filesystem.h"

// INIT, READ AND WRITE

struct Filesystem* read_filesystem(FILE* filesystem_file) {
    struct Filesystem* filesys = malloc(sizeof(struct Filesystem));

    filesys->number_of_files = 0;
    for (int i=0; i < ( MAX_BLOCKS_PER_FILE * MAX_FILE_NUMBER ); i++) {
            filesys->blocks[i].free = true;
    }
    // Set number of files
    fscanf(filesystem_file, "f%d", &filesys->number_of_files);

    //For each file
    for (int i=0; i < filesys->number_of_files; i++) {
        // Read file size
        fscanf(filesystem_file, "f%d", &filesys->files[i].file_size);

        int read_result = 2;
        while(read_result == 2) {
            // Read block metadata
            int block_index, block_size;
            char buffer[BLOCK_SIZE+1]; // +1 for \n
            read_result = fscanf(filesystem_file, "b%d%d", &block_index, &block_size);

            // End if the isnt more blocks
            if (read_result != 2) break;

            // Read block info
            fscanf(filesystem_file, "%s", buffer);

            // Set block metadata
            filesys->blocks[block_index].size = block_size;
            filesys->blocks[block_index].free = false;

            // Set block info via strncopy
            strncpy(filesys->blocks[block_index].buffer, buffer, block_size);

            // Append new index to index node list
            append(filesys->files[i].first_block_index, block_index);
        }
    }

    if (fclose(filesystem_file) != 0) {
        printf("Error closing file...\nCould not read file");
        exit(2);
    }
    
    return filesys;
}

void write_filesystem(struct Filesystem* filesys, char* filename) {
    FILE* filesystem_file = fopen(filename, "w");

    // Write Filesystem Info
    fprintf(filesystem_file, "%05d\n", filesys->number_of_files);
    // Write Files Metadata
    for (int i = 0; i < filesys->number_of_files; i++) {
        fprintf(filesystem_file, "f%05d\n", filesys->files[i].file_size);
        struct Index_Node* tmp = filesys->files[i].first_block_index;
        while (tmp != NULL) {
            // Block index
            fprintf(filesystem_file, "b%05d", tmp->index);
            // Block size
            fprintf(filesystem_file, "%05d", filesys->blocks[tmp->index].size);
            // Block data
            fprintf(filesystem_file, "%s", filesys->blocks[tmp->index].buffer);
            // End of block
            fprintf(filesystem_file, "\n");
            tmp = tmp->next;
        }
    }
    if (fclose(filesystem_file) != 0) {
        printf("Error closing file...\nCould not save %s", filename);
        exit(2);
    }

}

void init_filesystem(struct Filesystem* filesys, char* filesys_name) {
    FILE *filesys_file = fopen(filesys_name, "r");
    if (filesys_file) {
        filesys = read_filesystem(filesys_file);
    } else {
        filesys->number_of_files = 0;
        for (int i=0; i < ( MAX_BLOCKS_PER_FILE * MAX_FILE_NUMBER ); i++) {
            filesys->blocks[i].free = true;
        }
    }
}

// LINKED LIST FOR BLOCKS INDICES

struct Index_Node* create_node(int index) {
    struct Index_Node* new_node = malloc(sizeof(struct Index_Node));
    new_node->index = index;
    new_node->next = NULL;
    return new_node;
}

void append(struct Index_Node* start, int index) {
    struct Index_Node* new_node = create_node(index);

    if (start == NULL) {
        start = new_node;
        return;
    }

    struct Index_Node* last = start;
    while (last->next != NULL) {
        last = last->next;
    }

    last->next = new_node;
}

void free_list(struct Index_Node* start) {
    struct Index_Node* temp;

    while (start != NULL) {
        temp = start;
        start = start->next;
        free(temp);
    }
}

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
    filesys->files[filesys->number_of_files].file_size = size;
    filesys->files[filesys->number_of_files].block_size = ((size+(BLOCK_SIZE-1))/BLOCK_SIZE);
    
    int assigned = 0;
    for (int i=0; (i < (MAX_BLOCKS_PER_FILE*MAX_FILE_NUMBER) && assigned < filesys->files[filesys->number_of_files].block_size); i++) {
        if (filesys->blocks[i].free) {
            append(filesys->files[filesys->number_of_files].first_block_index, i);
            assigned++;
        }
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