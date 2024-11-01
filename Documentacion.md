# Documentación Filesys_Manager
*Victoria Sandí Barrantes, Naomi Ilama Gamboa, Luis Carlos Navarro Todd*

[Github Repo](https://github.com/luis-nav/filesys_manager)

### 1. **Estructuras de Datos Utilizadas**
   - **Index_Node y Bloques**: Las estructuras de datos principales para representar bloques del sistema de archivos se basan en una estructura de `Index_Node` y en la estructura de `Block`. Los Index_Node contienen el índice del bloque y funcionan como una lista enlazada que señala los bloques ocupados por un archivo. Los bloques se representan como estructuras separadas que contienen un array de bytes (de 512 bytes de tamaño), una variable de control para saber si está ocupado o disponible y el tamaño de los datos escritos dentro del bloque.
   - **File**: Para los archivos y y sus index_nodes, se emplea el `File` que contiene el nombre del archivo, su tamaño, su tamaño en número de bloques y un puntero al primer Index_Node del archivo.
   - **Filesystem**: Para manejar el sistema por completo se utiliza la estructura de `Filesystem` que simula tener todo el almacenamiento del sistema mediante los bloques y cuenta con un array de archivos, con su respectiva variable de control de tamaño.
   - **Estructuras para la Entrada del Usuario**: Existen estructuras de datos diseñadas para procesar las entradas y comandos del usuario. Estas estructuras están organizadas de manera modular para permitir el parseo de argumentos y líneas de comando del script seleccionado, utilizando componentes separados para la lógica de parseo.

### 2. **Decisiones Principales de Diseño**
   - **Modularidad**: El proyecto se ha diseñado de manera modular, con archivos separados (`arg_parser.c`, `line_parser.c`, `filesystem.c`, etc.) para gestionar diferentes aspectos del sistema. Por ejemplo, la lógica para parsear argumentos y comandos está claramente separada de las operaciones internas del sistema de archivos.
   - **Abstracción del Sistema de Archivos**: La implementación utiliza una capa de abstracción que separa los detalles de cómo se almacenan y manejan los bloques, archivos, nodos, y el sistema, de la lógica de entrada y salida del usuario. Esto se hace a través de funciones declaradas en `filesystem.h` y `filesys_manager.h`, manteniendo el sistema flexible y extensible.
