
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "matrix.h"
# include "finiteElement.h"

// void ProblemDef(const char *filename, int* dim, int* NNodes, int* NElements, int* NMaterials, 
// int* NNodes_Elemento, int* NCDirichlet, int* NCNewmann) {

//     FILE *file = fopen(filename, "r");
//     char line[256];

//     if (file == NULL) {
//         printf("No se pudo abrir el archivo\n");
//         return;
//     }

//     // Variable auxiliar
//     int aux = 0;

//     while (fgets(line, sizeof(line), file) != NULL) {
//         // Analizar la dimensión
//         if (strstr(line, "DIMENSIONES_DEL_PROBLEMA:")) {    
//             if (sscanf(line, "DIMENSIONES_DEL_PROBLEMA: %d", dim) != 1) {
//                 printf("Error al leer la dimensión del problema\n");
//                 fclose(file);
//                 return;
//             }
//         }
//         // Analizar el número de puntos nodales
//         else if (strstr(line, "NUMERO_DE_PUNTOS_NODALES:")) {
//             if (sscanf(line, "NUMERO_DE_PUNTOS_NODALES: %d", NNodes) != 1) {
//                 printf("Error al leer el número de puntos nodales\n");
//                 fclose(file);
//                 return;
//             }
//         }
//         // Analizar el número total de elementos
//         else if (strstr(line, "NUMERO_TOTAL_DE_ELEMENTOS:")) {
//             if (sscanf(line, "NUMERO_TOTAL_DE_ELEMENTOS: %d", NElements) != 1) {
//                 printf("Error al leer el número total de elementos\n");
//                 fclose(file);
//                 return;
//             }
//         }
//         // Analizar el número de materiales
//         else if (strstr(line, "NUMERO_DE_MATERIALES:")) {
//             if (sscanf(line, "NUMERO_DE_MATERIALES: %d", NMaterials) != 1) {
//                 printf("Error al leer el número de materiales\n");
//                 fclose(file);
//                 return;
//             }
//         }
//         // Analizar el número de nodos por elemento
//         else if (strstr(line, "NUMERO_DE_NODOS_ELEMENTO:")) {
//             if (sscanf(line, "NUMERO_DE_NODOS_ELEMENTO: %d", NNodes_Elemento) != 1) {
//                 printf("Error al leer el número de nodos por elemento\n");
//                 fclose(file);
//                 return;
//             }
//         }
//         // Analizar el número de condiciones de Dirichlet
//         else if (strstr(line, "Dirichlet:")) {
//             if (sscanf(line, "Dirichlet: %d", NCDirichlet) != 1) {
//                 printf("Error al leer el número de condiciones Dirichlet\n");
//                 fclose(file);
//                 return;
//             }
//         }
//         // Analizar el número de condiciones de Neumann
//         else if (strstr(line, "Newmann:")) {
//             if (sscanf(line, "Newmann: %d", NCNewmann) != 1) {
//                 printf("Error al leer el número de condiciones Neumann\n");
//                 fclose(file);
//                 return;
//             }
//         }

//         // Revisar tipo de elemento para revaluar dimensiones 
//         else if (strstr(line, "NUMERO_DE_ELEMENTOS_LI02:")) {
//             if (sscanf(line, "NUMERO_DE_ELEMENTOS_LI02: %d", &aux) != 1) {
//                 printf("Error al leer tipo de conectividad\n");
//                 fclose(file);
//                 return;
//             }

//             // Tipo de elemento linear
//             if (aux != 0){
//                 *dim = 1;
//             }

//         }

//     }

//     fclose(file);
// }

// // Rutina que lee la malla y da los parámetros del problema
// void Mesh(const char *filename, double **nodos, int** elementos, unsigned int* Materials, 
// int dim, int NNodes, int NElements, int NNodes_Elemento) {
//     FILE *file = fopen(filename, "r");
//     char line[256];

//     if (file == NULL) {
//         printf("No se pudo abrir el archivo '%s'\n", filename);
//         return;
//     }

//     unsigned int readingCoordinates = 0, readingElements = 0, nodeCount = 0, elementCount = 0;
//     unsigned int elementPos, CoordPos;

//     while (fgets(line, sizeof(line), file) != NULL) {
        
//         // printf("%s", line);

//         if (strstr(line, "End Coordinates")) {
//             readingCoordinates = 0;
//             continue;
//         } else if (strstr(line, "End Elements")) {
//             readingElements = 0;
//             continue;
//         }

//         if (strstr(line, "Coordinates")) {
//             readingCoordinates = 1;
//         } else if (strstr(line, "Elements")) {
//             readingElements = 1;
//         }

//         if (readingCoordinates == 1 && nodeCount == 0) {
//             // printf("%s", line);
//             // Lectura de vector de coordenadas
//             for (int i = 0; i < NNodes; i++) {
                
//                 // Posición de la coordenada
//                 if (fscanf(file, "%d", &CoordPos) != 1) { 
//                     fclose(file);
//                     return; // Error de lectura
//                 }

//                 // printf("Posición coordenada: %d\n", CoordPos);

//                 // Lee el valor del nodo y lo guarda
//                 // if (fscanf(file, "%lf", &nodos[i]) != 1) {
//                 //     fclose(file);
//                 //     return;  // Error de lectura
//                 // }

//                 // Caso 1d
//                 if (dim == 1){ 
//                     for (int j = 0; j < dim; j++){
//                         if (fscanf(file, "%lf", &nodos[i][j]) != 1) { // Asignar valores directamente a la matriz
//                             fclose(file);
//                             return; // Error de lectura
//                         }

//                         // Salta segundo valor nulo
//                         fscanf(file, "%*f");

//                         // printf("%lf\n", nodos[i][j]);

//                     }

//                 }

//                 // Caso 2d en adelante
//                 if (dim != 1){ 
//                     for (int j = 0; j < dim; j++){
//                         if (fscanf(file, "%lf", &nodos[i][j]) != 1) { // Asignar valores directamente a la matriz
//                             fclose(file);
//                             return; // Error de lectura
//                         }

//                         // printf("%lf\n", nodos[i][j]);

//                     }

//                 }
                
//             }

//             nodeCount ++;

//         } else if (readingElements == 1 && elementCount == 0) {
//             // printf("%s", line);
//             fgets(line, sizeof(line), file); // Información sobre elemento
//             // printf("Leído: %s", line);
//             // Lectura de matriz de conexiones
//             for (int i = 0; i < NElements; i++)
//             {
//                 // Posición del elemento
//                 if (fscanf(file, "%d", &elementPos) != 1) { 
//                     fclose(file);
//                     return; // Error de lectura
//                 }

//                 // printf("Posición elemento: %d\n", elementPos);

//                 // Leer material
//                 if (fscanf(file, "%d", &Materials[elementPos - 1]) != 1) { 
//                     fclose(file);
//                     return; // Error de lectura
//                 }

//                 // printf("Material : %d\n", Materials[elementPos-1]);

//                 for (int j = 0; j < NNodes_Elemento; j++)
//                 {
//                     if (fscanf(file, "%d", &elementos[elementPos - 1][j]) != 1) { // Asignar valores directamente a la matriz
//                         fclose(file);
//                         return; // Error de lectura
//                     }

//                     // printf("%d\n", elementos[i][j]);

//                 }

//                 // Salta una línea completa en el archivo
//                 // fscanf(file, "%*[^\n]\n");

//             }   
//             elementCount ++;

//         }
//     }

//     fclose(file);
// }


// Función para leer las condiciones del archivo
BoundaryConditions* readConditions(const char *filename, int NCDirichlet, int NCNeumann) {
    FILE *file = fopen(filename, "r");
    BoundaryConditions *bc = malloc(sizeof(BoundaryConditions));

    if (file == NULL || bc == NULL) {
        printf("Error al abrir el archivo o al asignar memoria.\n");
        return NULL;
    }

    // Inicializar los arrays dentro de la estructura BoundaryConditions
    bc->dirichletNodes = malloc(NCDirichlet * sizeof(int));
    bc->dirichletValues = malloc(NCDirichlet * sizeof(double));
    bc->neumannNodes = malloc(NCNeumann * sizeof(int));
    bc->neumannValues = malloc(NCNeumann * sizeof(double));

    char line[256];
    unsigned int readingDirichlet = 0, readingNeumann = 0;

    // Itera sobre las lineas del archivo
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strcmp(line, "Dirichlet\n") == 0) {
            readingDirichlet = 1;
            continue;  // Salta a la siguiente iteración del bucle para comenzar a leer condiciones
        } else if (strcmp(line, "Newmann\n") == 0) {
            readingNeumann = 1;
            continue;  // Salta a la siguiente iteración del bucle para comenzar a leer condiciones
        } else if (strstr(line, "End Dirichlet")) {
            readingDirichlet = 0;
        } else if (strstr(line, "End Newmann")) {
            readingNeumann = 0;
        } else if (readingDirichlet) {
            if (sscanf(line, "%d %lf", &bc->dirichletNodes[readingDirichlet - 1], &bc->dirichletValues[readingDirichlet - 1]) == 2) {
                readingDirichlet++;
                if (readingDirichlet > NCDirichlet) break;  // Rompe el bucle si se leyeron todas las condiciones esperadas
            }
        } else if (readingNeumann) {
            if (sscanf(line, "%d %lf", &bc->neumannNodes[readingNeumann - 1], &bc->neumannValues[readingNeumann - 1]) == 2) {
                readingNeumann++;
                if (readingNeumann > NCNeumann) break;  // Rompe el bucle si se leyeron todas las condiciones esperadas
            }
        }
    }

    fclose(file);
    return bc;
}



int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Uso: %s <nombre_archivo>\n", argv[0]);
        return 1;
    }

    const char* filename_dat = argv[1];
    int dim, NNodes, NElements, NNodes_Elemento, NMaterials, NCDirichlet, NCNeumann;

    ProblemDef(filename_dat, &dim, &NNodes, &NElements, &NMaterials, &NNodes_Elemento, 
    &NCDirichlet, &NCNeumann);

    // printf("Dimension: %d\n", dim);
    // printf("Numero de Nodos: %d\n", NNodes);
    // printf("Numero de Elementos: %d\n", NElements);
    // printf("Nodos por Elemento: %d\n", NNodes_Elemento);
    // printf("Numero de Materiales: %d\n", NMaterials);
    // printf("Condiciones Dirichlet: %d\n", NCDirichlet);
    // printf("Condiciones Neumann: %d\n", NCNeumann);

    double** nodos = createMatrix(NNodes, dim); // Matriz para almacenar las coordendas de los nodos
    int** elementos = malloc( NElements * sizeof(int *)); // Matriz para almacenar la conexión entre nodos para cada elemento
    unsigned int* Materials = malloc( NElements * sizeof(int)); // Matriz para almacenar los materiales

    for (int i = 0; i < NElements; i++) {
        // Asigna memoria para cada columna de esta fila
        elementos[i] = malloc(NNodes_Elemento * sizeof(int));
    }

    Mesh(filename_dat, nodos, elementos, Materials, dim, NNodes, NElements, NNodes_Elemento);

    // Mostrar las conexiones de los elementos
    // printf("Elementos y sus nodos conectados:\n");

    // for (int i = 0; i < NElements; i++) {
    //     printf("Elemento %d. Nodos: ", i+1);
    //     for (int j = 0; j < NNodes_Elemento; j++){     
    //         printf("%d   ", elementos[i][j]);     // Nodo inicial del elemento i
    //     }
    //     printf("\n");
    // }

    // printf("Coordendas\n");
    // MatrixShow(NNodes, dim, nodos);

    // printf("Materiales por nodo:\n");
    // for (int i = 0; i<NElements; i++){
    //     printf("Elemento %d. Material: %d\n", i+1, Materials[i]);
    // }

    // Leemos condiciones de contorno
    // Llamando a la función readConditions
    BoundaryConditions* bc = readConditions(filename_dat, NCDirichlet, NCNeumann);

    // Comprobar si se devolvió correctamente la estructura BoundaryConditions
    if (bc != NULL) {
        // La función se ejecutó correctamente, puedes usar los datos de 'bc' aquí

        // Ejemplo de acceso a los datos:
        printf("Condiciones Dirichlet:\n");
        for (int i = 0; i < NCDirichlet; i++) {
            printf("Nodo: %d, Valor: %f\n", bc->dirichletNodes[i], bc->dirichletValues[i]);
        }

        // No olvides liberar la memoria asignada dinámicamente
        free(bc->dirichletNodes);
        free(bc->dirichletValues);
        free(bc->neumannNodes); // Incluso si NCNeumann es 0, es buena práctica intentar liberar, ya que free(NULL) es seguro
        free(bc->neumannValues);
        free(bc);
    } else {
        printf("Error al leer las condiciones del archivo.\n");
    }


    // Liberar la memoria
    for (int i = 0; i < NElements; i++) {
        free(elementos[i]); // Libera cada fila de la matriz
    }
    free(elementos); // Libera el arreglo de filas
    freeMatrix(nodos, NNodes);
    free(Materials);

    return 0;
}