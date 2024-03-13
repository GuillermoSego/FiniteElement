/*

Biblioteca para el método de los elementos finitos

*/


# include <stdio.h>
# include <math.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>
# include "matrix.h"
# include "finiteElement.h"

// Rutina que genera una malla equidistante. Regresa un vector con la malla generada
void MallaGeneratorEquidist(int Dim, int NElements, double* malla, double x_0, double x_n){

    // Dimensión del problema
    if (Dim == 1) {
        linspace(malla, x_0, x_n, NElements);
    }

}

// Funciones de forma para elementos lineales 1D
double N1(double ei) {
    return (1.0 - ei) / 2.0;
}

double N2(double ei) {
    return (1.0 + ei) / 2.0;
}

double dN1de(double ei) {
    return -0.5;
}

double dN2de(double ei) {
    return 0.5;
}


// Función jacobiana en una dimensión
double invjacobian1d(double x1, double x2){
    return 2.0/(x2 - x1);
}

// Función que regresa el determinante del jacobiano en 1d
double detJacobian1d(double x1, double x2){
    return (x2 - x1)/2.0;
}

// Construcción de la matriz B para un elemento
void BuildB1d(double** B, double **DNDE, double x1, double x2, int dim, int NElements){

    double InvJac = 0; 
    // Llenado de la matriz B para un elemento
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < NElements; j++)
        {
            InvJac = invjacobian1d(x1, x2);
            B[i][j] = InvJac * DNDE[i][j]; // Las derivadas ya están evaluadas en los Gauss P
        }
    }

}

// Construcción del vector F para un elemento con Q constante
void BuildF1d(double* F, double** N, double Q, double x1, double x2, int NElements ){

    double detJac = 0;

    for (int i = 0; i < NElements; i++){

        // Calculamos el determinante del jacobiano
        detJac = detJacobian1d(x1, x2);

        // Calculamos el valor del vector F elemental
        F[i] = N[0][i]*Q*detJac;
    }
}


// Función para leer las condiciones del archivo
BoundaryConditions* readConditions(const char *filename) {
    FILE *file = fopen(filename, "r");
    BoundaryConditions *bc = malloc(sizeof(BoundaryConditions));
    char line[256];
    int numConditions;

    if (file == NULL || bc == NULL) {
        printf("Error al abrir el archivo o al asignar memoria.\n");
        return NULL;
    }

    // Leer el número de condiciones
    fgets(line, sizeof(line), file); // Linea de no de condiciones
    fgets(line, sizeof(line), file); // Linea del número
    sscanf(line, "%d", &numConditions);
    // printf("No condiciones: %d\n", numConditions);

    // Leer y procesar las condiciones de Dirichlet
    fgets(line, sizeof(line), file); // Leer la línea "Condiciones Dirichlet"
    // printf("Leído: %s\n", line);
    fgets(line, sizeof(line), file); // Leer la línea con las condiciones
    // printf("Leído: %s\n", line);
    line[strcspn(line, "\r\n")] = 0;  // Elimina el final de línea

    // printf("Lectura de condiciones\n");

    for (int i = 0; i < 2; i++) {
        char *token = (i == 0) ? strtok(line, ",") : strtok(NULL, ",");
        if (token == NULL) {
            break; // No hay más tokens
        }
        // printf("Leído: %s\n", token); // Después de leer una línea
        if (strcmp(token, "None") == 0) {
            bc->dirichletValid[i] = false;
        } else {
            bc->dirichlet[i] = atof(token);
            bc->dirichletValid[i] = true;
        }
    }

    // Leer y procesar las condiciones de Neumann
    fgets(line, sizeof(line), file); // Leer la línea "Condiciones Neumann"
    fgets(line, sizeof(line), file); // Leer la línea con las condiciones
    for (int i = 0; i < 2; i++) {
        char *token = (i == 0) ? strtok(line, ", ") : strtok(NULL, ", ");
        if (strcmp(token, "None") == 0) {
            bc->neumannValid[i] = false;
        } else {
            bc->neumann[i] = atof(token);
            bc->neumannValid[i] = true;
        }
    }

    fclose(file);
    return bc;
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