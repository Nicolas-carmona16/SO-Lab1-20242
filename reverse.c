/* ********************************************
 * Integrantes:                               *
 * - Efraín García Valencia                   *
 * - Nicolás Carmona Cardona                  *
 * ********************************************/

#include <stdio.h>   // Para funciones de entrada/salida estándar
#include <stdlib.h>  // Para funciones como malloc, free, exit
#include <string.h>  // Para funciones de manipulación de cadenas
#include <sys/stat.h>   // Para usar la función stat()

#define INITIAL_CAPACITY 10  // Capacidad inicial del array dinámico

#ifdef _WIN32
typedef long ssize_t;  // Definir ssize_t en sistemas Windows para compatibilidad
#endif

// Función para verificar si dos archivos son el mismo
int verificar_archivos(const char *file1, const char *file2) {
    struct stat stat1, stat2;   // Estructuras para almacenar información de archivos

    // Obtener información del primer archivo
    if (stat(file1, &stat1) != 0) {
        return 0;   // No se pudo obtener la información del archivo
    }

    // Obtener información del segundo archivo
    if (stat(file2, &stat2) != 0) {
        return 0;   // No se pudo obtener la información del archivo
    }

    // Comparar el número de dispositivo e inodo
    return (stat1.st_dev == stat2.st_dev && stat1.st_ino == stat2.st_ino); // Devuelve si son iguales
}

// Función para invertir el orden de las líneas
void reverseLines(char **lines, size_t count) {
    for (size_t i = 0; i < count / 2; ++i) {
        // Intercambiar punteros de líneas
        char *temp = lines[i];
        lines[i] = lines[count - i - 1];
        lines[count - i - 1] = temp;
    }
}

int main(int argc, char *argv[]) {
    FILE *input = stdin;   // Archivo de entrada por defecto: entrada estándar
    FILE *output = stdout; // Archivo de salida por defecto: salida estándar
    char **lines = NULL;   // Array dinámico de punteros a líneas
    size_t capacity = INITIAL_CAPACITY;  // Capacidad inicial del array
    size_t count = 0;      // Número de líneas leídas
    char *line = NULL;     // Buffer para almacenar temporalmente cada línea
    size_t len = 0;        // Tamaño del buffer (usado por getline)
    ssize_t read;          // Resultado de getline()

    // Verificar número de argumentos
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    // Abrir archivo de entrada si se proporciona
    if (argc >= 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    }

    // Abrir archivo de salida si se proporciona
    if (argc == 3) {
        // Verificar que los archivos de entrada y salida sean diferentes
        if (strcmp(argv[1], argv[2]) == 0 || verificar_archivos(argv[1], argv[2])) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
        output = fopen(argv[2], "w");
        if (!output) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
            exit(1);
        }
    }

    // Reservar memoria inicial para el array de líneas
    lines = malloc(capacity * sizeof(char *));
    if (!lines) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    // Leer líneas desde el archivo de entrada o stdin
    while ((read = getline(&line, &len, input)) != -1) {
        // Si el array está lleno, aumentar su capacidad
        if (count == capacity) {
            capacity *= 2;  // Duplicar la capacidad
            char **new_lines = realloc(lines, capacity * sizeof(char *)); // realloc, redimenciona el array
            if (!new_lines) {
                fprintf(stderr, "realloc failed\n");
                exit(1);
            }
            lines = new_lines;
        }
        lines[count++] = line;  // Almacenar el puntero de la línea leída
        line = NULL;  // Asegurar que getline asigne un nuevo buffer en la siguiente llamada
    }

    free(line);  // Liberar el último buffer si se utilizó

    // Invertir el orden de las líneas
    reverseLines(lines, count);

    // Escribir las líneas invertidas al archivo de salida o stdout
    for (size_t i = 0; i < count; ++i) {
        // Verificar si la línea no termina con un salto de línea y agregarlo si es necesario
        if (lines[i][strlen(lines[i]) - 1] != '\n') {
            fprintf(output, "%s\n", lines[i]);  // Agregar manualmente el salto de línea
        } else {
            fputs(lines[i], output);  // Imprimir la línea tal cual
        }
        free(lines[i]);  // Liberar la memoria de cada línea después de escribirla
    }
    free(lines);  // Liberar el array de punteros

    // Cerrar archivos si se abrieron
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);

    return 0;
}