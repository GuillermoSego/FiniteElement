#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Rutina que lee la malla y da los parámetros del problema
void ProblemDefinition(const char *filename, int* dim, int* NNodes, int* NElements, int* NNodes_Elemento){
    FILE *file;
    file = fopen(filename, "r");

    char line[256];

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return; 
    }

    fgets(line, sizeof(line), file); // Línea de condiciones
    // printf("Leído: %s", line);

    // Analiza la línea para extraer la dimensión y el número de nodos por elemento
    if (sscanf(line, "MESH dimension %d ElemType Linear Nnode %d", dim, NNodes_Elemento) == 2) {
        // Si sscanf devuelve 2, significa que ambos valores fueron leídos exitosamente
        // printf("La dimensión de la malla es: %d\n", *dim);
        // printf("El número de nodos por elemento: %d\n", *NNodes_Elemento);
    } else {
        printf("No se pudo leer correctamente la información de la malla\n");
        return;
    }

    *NNodes = 0; // Inicialización de contadores
    *NElements = 0;
    int readingCoordinates = 0, readingElements = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, "End Coordinates")) {
            readingCoordinates = 0;
            continue;
        } else if (strstr(line, "End Elements")) {
            readingElements = 0;
            continue;
        }

        if (strstr(line, "Coordinates")) {
            readingCoordinates = 1; // Iniciar la lectura de coordenadas
            continue; // Saltar al siguiente ciclo del bucle
        } else if (strstr(line, "Elements")) {
            readingElements = 1; // Iniciar la lectura de elementos
            readingCoordinates = 0; // Asegurar que la lectura de coordenadas esté desactivada
            continue;
        }

        if (readingCoordinates) {
            (*NNodes)++; // Cada línea válida dentro de Coordinates incrementa el contador de nodos
        } else if (readingElements) {
            (*NElements)++; // Cada línea válida dentro de Elements incrementa el contador de elementos
        }
    }

    // printf("Número total de nodos: %d\n", *NNodes);
    // printf("Número total de elementos: %d\n", *NElements);

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <nombre_archivo>\n", argv[0]);
        return 1;
    }

    const char* filename_Mesh = argv[1];
    int dim, NNodos, NElementos, NNodes_Elemento;

    ProblemDefinition(filename_Mesh, &dim, &NNodos, &NElementos, &NNodes_Elemento);

    printf("Dimension: %d\n", dim);
    printf("Numero de Nodos: %d\n", NNodos);
    printf("Numero de Elementos: %d\n", NElementos);
    printf("Nodos por Elemento: %d\n", NNodes_Elemento);

    return 0;
}
