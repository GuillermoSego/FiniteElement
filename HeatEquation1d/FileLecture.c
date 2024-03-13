#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "matrix.h"

// Esta rutina muestra un vector de mxn
void VectorShow(int m, int n, double *v){

    int i, j, k=0;
    
    for(i=0 ; i<m ; i++){
        
        for (j=0 ; j<n ; j ++){
            
            printf("%lf \t", v[k]);
            k ++;
        }
        
        printf("\n");
    }
    
    
}

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

// Rutina que lee la malla y da los parámetros del problema
void ReadMesh(const char *filename, double *nodos, int** elementos, int dim, int NNodes, int NElements, int NNodes_Elemento) {
    FILE *file = fopen(filename, "r");
    char line[256];

    if (file == NULL) {
        printf("No se pudo abrir el archivo '%s'\n", filename);
        return;
    }

    unsigned int readingCoordinates = 0, readingElements = 0, nodeCount = 0, elementCount = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        
        // printf("%s", line);

        if (strstr(line, "End Coordinates")) {
            readingCoordinates = 0;
            continue;
        } else if (strstr(line, "End Elements")) {
            readingElements = 0;
            continue;
        }

        if (strstr(line, "Coordinates")) {
            readingCoordinates = 1;
        } else if (strstr(line, "Elements")) {
            readingElements = 1;
            readingCoordinates = 0;
        }

        if (readingCoordinates == 1 && nodeCount == 0) {
            // printf("%s", line);
            // Lectura de vector de coordenadas
                for (int i = 0; i < NNodes; i++) {
                    // Salta el primer valor
                    fscanf(file, "%*f");

                    // Lee el valor del nodo y lo guarda
                    if (fscanf(file, "%lf", &nodos[i]) != 1) {
                        fclose(file);
                        return;  // Error de lectura
                    }
                    // printf("%lf\n", nodos[i]);
                }

            nodeCount ++;

        } else if (readingElements == 1 && elementCount == 0) {
            // printf("%s", line);
            // Lectura de matriz de conexiones
            for (int i = 0; i < NElements; i++)
            {
                // Salta el primer valor
                fscanf(file, "%*d");

                for (int j = 0; j < NNodes_Elemento; j++)
                {
                    if (fscanf(file, "%d", &elementos[i][j]) != 1) { // Asignar valores directamente a la matriz
                        fclose(file);
                        return; // Error de lectura
                    }

                    // printf("%d\n", elementos[i][j]);

                }

                // Salta una línea completa en el archivo
                // fscanf(file, "%*[^\n]\n");

            }   
            elementCount ++;

        }
    }

    fclose(file);
}



int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <nombre_archivo>\n", argv[0]);
        return 1;
    }

    const char* filename_Mesh = argv[1];
    int dim, NNodes, NElements, NNodes_Elemento;

    ProblemDefinition(filename_Mesh, &dim, &NNodes, &NElements, &NNodes_Elemento);

    printf("Dimension: %d\n", dim);
    printf("Numero de Nodos: %d\n", NNodes);
    printf("Numero de Elementos: %d\n", NElements);
    printf("Nodos por Elemento: %d\n", NNodes_Elemento);

    double* nodos = malloc(NNodes * sizeof(double)); // Arreglo para almacenar las coordenadas de los nodos
    int** elementos = malloc( NElements * sizeof(int *)); // Matriz para almacenar la conexión entre nodos para cada elemento

    for (int i = 0; i < NElements; i++) {
        // Asigna memoria para cada columna de esta fila
        elementos[i] = malloc(NNodes_Elemento * sizeof(int));
    }

    ReadMesh(filename_Mesh, nodos, elementos, dim, NNodes, NElements, NNodes_Elemento);

    // Mostrar las coordenadas de los nodos
    VectorShow(NNodes, 1, nodos);

    // Mostrar las conexiones de los elementos
    printf("Elementos y sus nodos conectados:\n");

    for (int i = 0; i < NElements; i++) {
        printf("Elemento %d. Nodos: ", i+1);
        for (int j = 0; j < NNodes_Elemento; j++){     
            printf("%d   ", elementos[i][j]);     // Nodo inicial del elemento i
        }
        printf("\n");
    }


    // Liberar la memoria
    for (int i = 0; i < NElements; i++) {
        free(elementos[i]); // Libera cada fila de la matriz
    }
    free(elementos); // Libera el arreglo de filas
    free(nodos); 


    return 0;
}
