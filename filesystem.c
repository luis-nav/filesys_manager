#include "filesystem.h"

// Lee el sistema de archivos desde el archivo dado y devuelve un puntero a Filesystem
struct Filesystem* read_filesystem(FILE* file) {
    struct Filesystem* filesys = malloc(sizeof(struct Filesystem)); // reserva memoria para la estructura Filesystem
    fread(filesys, sizeof(struct Filesystem), 1, file); // lee el contenido del archivo en la estructura
    fclose(file); // cierra el archivo después de leer
    return filesys;
}

// Guarda el estado actual del sistema de archivos en el archivo indicado
void write_filesystem(struct Filesystem* filesys, char* filename) {
    FILE* file = fopen(filename, "w"); // abre el archivo en modo escritura
    if (file == NULL) { // valida si el archivo fue abierto correctamente
        perror("Error al abrir el archivo de sistema de archivos. Exiting the app...");
        exit(1);
    }
    fwrite(filesys, sizeof(struct Filesystem), 1, file); // guarda el contenido de Filesystem en el archivo
    fclose(file); // cierra el archivo después de escribir
}

// Crea un nuevo nodo para la lista enlazada de índices de bloques
struct Index_Node* create_node(int index) {
    struct Index_Node* new_node = malloc(sizeof(struct Index_Node)); // reserva memoria para el nuevo nodo
    new_node->index = index; // asigna el índice al nuevo nodo
    new_node->next = NULL; // inicializa el siguiente nodo como NULL
    return new_node;
}

// Agrega un nodo al final de la lista de índices de bloques
void append(struct Index_Node* start, int index) {
    struct Index_Node* new_node = create_node(index); // crea un nuevo nodo con el índice proporcionado

    if (start == NULL) { // si la lista está vacía, asigna el nodo como el inicio
        start = new_node;
        return;
    }

    struct Index_Node* last = start; // encuentra el último nodo en la lista
    while (last->next != NULL) {
        last = last->next;
    }

    last->next = new_node; // agrega el nuevo nodo al final de la lista
}

// Libera la memoria ocupada por la lista de índices de bloques
void free_list(struct Index_Node* start) {
    struct Index_Node* temp;

    while (start != NULL) { // recorre la lista y libera cada nodo
        temp = start;
        start = start->next;
        free(temp);
    }
}

// Inicializa el sistema de archivos; lee de un archivo o establece valores por defecto si no existe
void init_filesystem(struct Filesystem* filesys, char* filesys_name) {
    FILE *filesys_file = fopen(filesys_name, "r"); // intenta abrir el archivo del sistema de archivos en modo lectura
    if (filesys_file) { // si existe, lee el sistema de archivos desde el archivo
        filesys = read_filesystem(filesys_file);
    } else { // si no existe, inicializa un nuevo sistema de archivos
        filesys->number_of_files = 0;
        for (int i=0; i < ( MAX_BLOCKS_PER_FILE * MAX_FILE_NUMBER ); i++) { // marca todos los bloques como libres
            filesys->blocks[i].free = true;
        }
    }
}

// Crea un archivo en el sistema de archivos
void create_file(struct Filesystem* filesys, char* filename, int size) {
    // verifica límites de archivos
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

    // configura el nuevo archivo con el nombre, tamaño y bloques necesarios
    filesys->files[filesys->number_of_files].name = filename;
    filesys->files[filesys->number_of_files].file_size = size;
    filesys->files[filesys->number_of_files].block_size = ((size+(BLOCK_SIZE-1))/BLOCK_SIZE);
    
    int assigned = 0; // cantidad de bloques asignados al archivo
    for (int i=0; (i < (MAX_BLOCKS_PER_FILE*MAX_FILE_NUMBER) && assigned < filesys->files[filesys->number_of_files].block_size); i++) {
        if (filesys->blocks[i].free) { // si el bloque está libre, se asigna al archivo
            append(filesys->files[filesys->number_of_files].first_block_index, i);
            assigned++;
        }
    }
    filesys->number_of_files++; // incrementa el contador de archivos
}

// Escribe datos en un archivo desde un offset específico
void write_file(struct Filesystem* filesys, char* filename, int offset, char* data) {
    struct File* file = NULL;
    for (int i = 0; i < filesys->number_of_files; i++) { // busca el archivo con el nombre dado
        if (strcmp(filesys->files[i].name, filename) == 0) {
            file = &filesys->files[i];
            break;
        }
    }

    if (!file) { // valida que el archivo existe
        printf("Error: Archivo no encontrado. Exiting the app...");
        exit(1);
    }

    // verifica que el tamaño de los datos no exceda el tamaño del archivo
    if (offset + strlen(data) > file->file_size) {
        printf("Error: Excede el tamaño del archivo. Exiting the app...");
        exit(1);
    }

    // busca el bloque donde comenzará la escritura y ajusta el offset
    struct Index_Node* current_block = file->first_block_index;
    int block_index = offset / BLOCK_SIZE;
    for (int i = 0; i < block_index; i++) {
        current_block = current_block->next;
    }

    int block_offset = offset % BLOCK_SIZE;
    int data_written = 0; // variable que indica cuántos datos se han escrito
    while (data_written < strlen(data)) { // continúa hasta que todos los datos estén escritos
        int space_in_block = BLOCK_SIZE - block_offset;
        int data_to_write = strlen(data) - data_written;
        int write_size = (data_to_write < space_in_block) ? data_to_write : space_in_block;
        strncpy(filesys->blocks[current_block->index].buffer + block_offset, data + data_written, write_size);
        data_written += write_size;
        block_offset = 0;
        current_block = current_block->next;
    }
}

// Lee datos de un archivo desde un offset específico
void read_file(struct Filesystem* filesys, char* filename, int offset, int size) {
    struct File* file = NULL; // se inicializa la variable en null para validar en caso de que no se encuentre el archivo
    for (int i = 0; i < filesys->number_of_files; i++) { // se recorren todos los archivos
        if (strcmp(filesys->files[i].name, filename) == 0) { // caso 1: si se encuentra el archivo se guarda el archivo en la var file
            file = &filesys->files[i];
            break;
        }
    }

    if (!file) { // caso 2: si no se encuentra el archivo se manda un mensaje de error
        printf("Error: Archivo no encontrado. Exiting the app...");
        exit(1);
    }

    // validación de offset
    if (offset + size > file->file_size) { // manda mensaje de error en caso de que se exceda el tamaño del archivo
        printf("Error: Lectura excede el tamaño del archivo. Exiting the app...");
        exit(1);
    }

    struct Index_Node* current_block = file->first_block_index; // Obtiene el primer bloque del archivo
    int block_index = offset / BLOCK_SIZE; // Calcula el índice del bloque de inicio a partir del offset

    // Avanza al bloque correcto basado en el índice calculado
    for (int i = 0; i < block_index; i++) {
        current_block = current_block->next;
    }

    int block_offset = offset % BLOCK_SIZE; // Calcula el offset dentro del bloque inicial
    int data_read = 0; // Inicializa el contador de bytes leídos
    char buffer[size + 1]; // Crea un buffer para almacenar los datos leídos, con espacio adicional para el terminador nulo
    buffer[size] = '\0'; // Agrega el terminador nulo al final del buffer

    // Lee los datos en bloques hasta completar el tamaño solicitado
    while (data_read < size) {
        int space_in_block = BLOCK_SIZE - block_offset; // Calcula el espacio disponible en el bloque actual
        int data_to_read = size - data_read; // Calcula los datos restantes por leer
        int read_size = (data_to_read < space_in_block) ? data_to_read : space_in_block; // Define cuánto leer en esta iteración

        // Copia la porción de datos desde el bloque actual al buffer
        strncpy(buffer + data_read, filesys->blocks[current_block->index].buffer + block_offset, read_size);
        data_read += read_size; // Actualiza el contador de datos leídos
        block_offset = 0; // Resetea el offset para siguientes bloques
        current_block = current_block->next; // Pasa al siguiente bloque
    }

    printf("Salida: %s\n", buffer); // Imprime el contenido leído desde el archivo
}

// Elimina un archivo del sistema de archivos y libera sus bloques
void delete_file(struct Filesystem* filesys, char* filename) {
    int file_index = -1; // se inicializa la variable en -1 para validar en caso de que no se encuentre el archivo
    for (int i = 0; i < filesys->number_of_files; i++) { // se recorren todos los archivos
        if (strcmp(filesys->files[i].name, filename) == 0)

 { // caso 1: si se encuentra el archivo se setea file_index con el valor de i
            file_index = i;
            break;
        }
    }

    if (file_index == -1) { // caso 2: si no se encuentra el archivo se manda un mensaje de error
        printf("Error: Archivo no encontrado. Exiting the app...");
        exit(1);
    }

    struct Index_Node* block = filesys->files[file_index].first_block_index; // se busca el índice del primer bloque del archivo
    while (block) { // bucle que libera los bloques que contenían el archivo (se detiene cuando block es null)
        filesys->blocks[block->index].free = true;
        struct Index_Node* next = block->next;
        free(block);
        block = next;
    }

    for (int i = file_index; i < filesys->number_of_files - 1; i++) { // se borra el índice del archivo de la lista enlazada
        filesys->files[i] = filesys->files[i + 1];
    }

    filesys->number_of_files--; // se resta 1 a la cantidad de archivos después de borrar
}

// Lista todos los archivos del sistema de archivos
void list_files(struct Filesystem* filesys) {
    int num = filesys->number_of_files; // obtiene la cantidad de archivos
    if (num == 0) { // caso 1: no hay archivos
        printf("(No hay archivos)\n"); // avisa que no hay archivos
        return;
    }
    for (int i = 0; i < num; i++) { // caso 2: hay i archivos
        printf("%s - %d bytes\n", filesys->files[i].name, filesys->files[i].file_size); // imprime el nombre y el tamaño de cada archivo
    }
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