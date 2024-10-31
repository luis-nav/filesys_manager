#include "filesystem.h"

// INIT, READ AND WRITE

void read_filesystem(struct Filesystem* filesys, FILE* filesystem_file) {
    filesys->number_of_files = 0;
    for (int i=0; i < ( MAX_BLOCKS_PER_FILE * MAX_FILE_NUMBER ); i++) {
            filesys->blocks[i].free = true;
    }
    for (int i=0; i < MAX_FILE_NUMBER; i++) {
            filesys->files[i].first_block_index = NULL;
    }
    // Set number of files
    fscanf(filesystem_file, "%d", &(filesys->number_of_files));

    //For each file
    for (int i=0; i < filesys->number_of_files; i++) {
        // Read file metadata
        int filename_size = 0;
        fscanf(filesystem_file, "\nf %d %d ", &filesys->files[i].file_size, &filename_size);
        filesys->files[i].name[FILENAME_SIZE-1] = '\0'; 
        // fscanf(filesystem_file, "%s\n", filesys->files[i].name);
        fgets(filesys->files[i].name, filename_size+1, filesystem_file);

        int read_result = 2;
        while(read_result == 2) {
            // Read block metadata
            int block_index, block_size = 0;
            char buffer[BLOCK_SIZE+1]; // +1 for \n
            read_result = fscanf(filesystem_file, "\nb %d %d ", &block_index, &block_size);

            // End if the isnt more blocks
            if (read_result != 2) break;


            // Set block metadata
            filesys->blocks[block_index].size = block_size;
            filesys->blocks[block_index].free = false;

            // Read block info
            fgets(filesys->blocks[block_index].buffer, block_size+1, filesystem_file);

            // Append new index to index node list
            append(&filesys->files[i], block_index);
        }
    }

    if (fclose(filesystem_file) != 0) {
        printf("Error closing file...\nCould not read file");
        exit(2);
    }
}

// Guarda el estado actual del sistema de archivos en el archivo indicado
void write_filesystem(struct Filesystem* filesys, char* filename) {
    FILE* filesystem_file = fopen(filename, "w");

    // Write Filesystem Info
    fprintf(filesystem_file, "%02d\n", filesys->number_of_files);
    // Write Files Metadata
    for (int i = 0; i < filesys->number_of_files; i++) {
        fprintf(filesystem_file, "f %08d %03d %s\n", filesys->files[i].file_size, strlen(filesys->files[i].name),filesys->files[i].name);
        struct Index_Node* tmp = filesys->files[i].first_block_index;
        while (tmp != NULL) {
            // Block index
            fprintf(filesystem_file, "b %05d", tmp->index);
            // Block size
            fprintf(filesystem_file, " %05d ", filesys->blocks[tmp->index].size);
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

// LINKED LIST FOR BLOCKS INDICES

struct Index_Node* create_node(int index) {
    struct Index_Node* new_node = (struct Index_Node*)malloc(sizeof(struct Index_Node)); // reserva memoria para el nuevo nodo
    new_node->index = index; // asigna el índice al nuevo nodo
    new_node->next = NULL; // inicializa el siguiente nodo como NULL
    return new_node;
}

// Agrega un nodo al final de la lista de índices de bloques
void append(struct File* file, int index) {
    struct Index_Node* new_node = create_node(index); // crea un nuevo nodo con el índice proporcionado

    if (file->first_block_index == NULL) { // si la lista está vacía, asigna el nodo como el inicio
        file->first_block_index = new_node;
        return;
    }

    struct Index_Node* last = file->first_block_index; // encuentra el último nodo en la lista
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
        read_filesystem(filesys, filesys_file);
    } else { // si no existe, inicializa un nuevo sistema de archivos
        // filesys = malloc(sizeof(struct Filesystem));
        filesys->number_of_files = 0;
        for (int i=0; i < ( MAX_BLOCKS_PER_FILE * MAX_FILE_NUMBER ); i++) { // marca todos los bloques como libres
            filesys->blocks[i].free = true;
        }
        for (int i=0; i < MAX_FILE_NUMBER; i++) {
            filesys->files[i].first_block_index = NULL;
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
    strcpy(filesys->files[filesys->number_of_files].name, filename);
    filesys->files[filesys->number_of_files].file_size = size;
    filesys->files[filesys->number_of_files].block_size = ((size+(BLOCK_SIZE-1))/BLOCK_SIZE);
    
    int assigned = 0; // cantidad de bloques asignados al archivo
    int index = 0;
    while (assigned < filesys->files[filesys->number_of_files].block_size) {
        if (filesys->blocks[index].free) { // si el bloque está libre, se asigna al archivo
            append(&filesys->files[filesys->number_of_files], index);
            assigned++;
        }
        index++;
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
        printf("Error: File not found. Exiting the app...");
        exit(1);
    }

    // verifica que el tamaño de los datos no exceda el tamaño del archivo
    int data_length = strlen(data); // variable que indica largo de datos
    if (offset + data_length > file->file_size) {
        printf("Error: Data exceeds file limits. Exiting the app...");
        exit(1);
    }

    // busca el bloque donde comenzará la escritura y ajusta el offset
    struct Index_Node* block_index_node = file->first_block_index;
    int block_index_offset = offset / BLOCK_SIZE;
    for (int i=0; i<block_index_offset; i++) {
        block_index_node = block_index_node->next;
    }

    int block_offset = offset % BLOCK_SIZE; // Offset a nivel interno del bloque
    int data_written = 0; // variable que indica cuántos datos se han escrito
    while (data_written < data_length) { // continúa hasta que todos los datos estén escritos        
        int data_to_write = data_length - data_written;
        int write_size = (data_to_write < BLOCK_SIZE) ? data_to_write : BLOCK_SIZE;
        filesys->blocks[block_index_node->index].free = false;
        filesys->blocks[block_index_node->index].size = write_size;
        strncpy(&(filesys->blocks[block_index_node->index].buffer[block_offset]), &(data[0+data_written]), write_size);
        if (block_offset != 0)  block_offset = 0; // Si ya el offset se aplico, se setea en 0
        block_index_node = block_index_node->next; // Setear siguiente bloque
        data_written += write_size; // Agregar cuantos datos se han escrito
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
        printf("Error: File not found. Exiting the app...");
        exit(1);
    }

    // validación de offset
    if (offset + size > file->file_size) { // manda mensaje de error en caso de que se exceda el tamaño del archivo
        printf("Error: Read exceeds size of file. Exiting the app...");
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

    printf("Output: %s\n\n", buffer); // Imprime el contenido leído desde el archivo
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
        printf("Error: File not found. Exiting the app...");
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
        printf("(No Files)\n\n"); // avisa que no hay archivos
        return;
    }
    for (int i = 0; i < num; i++) { // caso 2: hay i archivos
        printf("%s - %d bytes\n", filesys->files[i].name, filesys->files[i].file_size); // imprime el nombre y el tamaño de cada archivo
    }
    printf("\n");
}
