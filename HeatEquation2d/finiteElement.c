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

// Funciones de forma para elementos lineales 2D
double N1(double xi, double eta) {
    return 1 - xi - eta;
}
double N2(double xi, double eta) {
    return xi;
}
double N3(double xi, double eta) {
    return eta;
}

double dN1de(double ei, double n) {
    return -1.0;
}
double dN2de(double ei, double n) {
    return 1.0;
}
double dN3de(double ei, double n) {
    return 0.0;
}
double dN1dn(double ei, double n) {
    return -1.0;
}
double dN2dn(double ei, double n) {
    return 0.0;
}
double dN3dn(double ei, double n) {
    return 1.0;
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

double DetJacobian(double** J, int dim) {
    double det = 0.0;

    switch (dim) {
        case 2:
            det = J[0][0] * J[1][1] - J[0][1] * J[1][0];
            break;

        case 3: {
            // Declara las variables al principio del bloque
            double a = J[0][0], b = J[0][1], c = J[0][2];
            double d = J[1][0], e = J[1][1], f = J[1][2];
            double g = J[2][0], h = J[2][1], i = J[2][2];

            det = a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
            break;
        }

        default:
            printf("Dimension no soportada.\n");
            break;
    }
    return det;
}




// Construimos la matriz jacobiana
void BuildJacobian(double** J, double** DNDE, double** coord, int dim, int NNodes_Element){

    /* 
    El jacobino es la multiplicación de las coordenadas por la matriz de 
    derivadas evaluadas en los puntos de gsuss
    */
    double** xT = createMatrix(NNodes_Element, dim);
    
    MatrixT(dim, NNodes_Element, coord, xT);

    // Calculamos el jacobiano
    MatrixProduct(DNDE, xT, J, dim, NNodes_Element, dim);

    // Libermaos memoria reservada
    freeMatrix(xT, NNodes_Element);

}

// Calcula la inversa del jacobiano
void InvJacobian(double** J, double** InvJ, double det, int dim){

    switch (dim)
    {
    case 2:

        // Calcula la inversa
        InvJ[0][0] = J[1][1] / det;
        InvJ[0][1] = -J[0][1] / det;
        InvJ[1][0] = -J[1][0]/ det;
        InvJ[1][1] = J[0][0] / det;
        
        break;
    
    case 3:
        break;
    }

}

// Construcción del vector F para un elemento con Q constante
void BuildF(double* F, double** N, double Q, double detJ, int NElements ){

    for (int i = 0; i < NElements; i++){

        // Calculamos el valor del vector F elemental
        F[i] = N[0][i]*Q*detJ;
    }
}


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

// Rutina que lee la malla y da los parámetros del problema
void ProblemDef(const char *filename, int* dim, int* NNodes, int* NElements, int* NMaterials, 
char** ElemType, int* NNodes_Elemento, int* NCDirichlet, int* NCNewmann) {

    FILE *file = fopen(filename, "r");
    char line[256];

    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }

    // Variable auxiliar
    int aux = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        // Analizar la dimensión
        if (strstr(line, "DIMENSIONES_DEL_PROBLEMA:")) {    
            if (sscanf(line, "DIMENSIONES_DEL_PROBLEMA: %d", dim) != 1) {
                printf("Error al leer la dimensión del problema\n");
                fclose(file);
                return;
            }
        }
        // Analizar el número de puntos nodales
        else if (strstr(line, "NUMERO_DE_PUNTOS_NODALES:")) {
            if (sscanf(line, "NUMERO_DE_PUNTOS_NODALES: %d", NNodes) != 1) {
                printf("Error al leer el número de puntos nodales\n");
                fclose(file);
                return;
            }
        }
        // Analizar el número total de elementos
        else if (strstr(line, "NUMERO_TOTAL_DE_ELEMENTOS:")) {
            if (sscanf(line, "NUMERO_TOTAL_DE_ELEMENTOS: %d", NElements) != 1) {
                printf("Error al leer el número total de elementos\n");
                fclose(file);
                return;
            }
        }
        // Analizar el número de materiales
        else if (strstr(line, "NUMERO_DE_MATERIALES:")) {
            if (sscanf(line, "NUMERO_DE_MATERIALES: %d", NMaterials) != 1) {
                printf("Error al leer el número de materiales\n");
                fclose(file);
                return;
            }
        }
        // Analizar el tipo de elementos en la malla
        else if (strstr(line, "TIPO_DE_ELEMENTOS_EN_LA_MALLA:")) {
            // Extrae primero el tipo de elemento en un buffer temporal
            char tempType[100]; // Asegúrate de que este tamaño sea suficiente
            if (sscanf(line, "TIPO_DE_ELEMENTOS_EN_LA_MALLA: %99s", tempType) == 1) {
                // Asigna memoria para ElemType
                *ElemType = malloc(strlen(tempType) + 1); // +1 para el carácter nulo
                if (*ElemType != NULL) {
                    strcpy(*ElemType, tempType); // Copia la cadena al espacio asignado
                } else {
                    printf("Error al asignar memoria para el tipo de elementos\n");
                    fclose(file);
                    return;
                }
            } else {
                printf("Error al leer el tipo de elementos en la malla\n");
                fclose(file);
                return;
            }
        }
        // Analizar el número de nodos por elemento
        else if (strstr(line, "NUMERO_DE_NODOS_ELEMENTO:")) {
            if (sscanf(line, "NUMERO_DE_NODOS_ELEMENTO: %d", NNodes_Elemento) != 1) {
                printf("Error al leer el número de nodos por elemento\n");
                fclose(file);
                return;
            }
        }
        // Analizar el número de condiciones de Dirichlet
        else if (strstr(line, "NCDirichlet:")) {
            if (sscanf(line, "NCDirichlet: %d", NCDirichlet) != 1) {
                printf("Error al leer el número de condiciones Dirichlet\n");
                fclose(file);
                return;
            }
        }
        // Analizar el número de condiciones de Neumann
        else if (strstr(line, "NCNewmann:")) {
            if (sscanf(line, "NCNewmann: %d", NCNewmann) != 1) {
                printf("Error al leer el número de condiciones Neumann\n");
                fclose(file);
                return;
            }
        }

        // Revisar tipo de elemento para revaluar dimensiones 
        else if (strstr(line, "NUMERO_DE_ELEMENTOS_LI02:")) {
            if (sscanf(line, "NUMERO_DE_ELEMENTOS_LI02: %d", &aux) != 1) {
                printf("Error al leer tipo de conectividad\n");
                fclose(file);
                return;
            }

            // Tipo de elemento linear
            if (aux != 0){
                *dim = 1;
            }

        }

    }

    fclose(file);
}

// Rutina que lee la malla y da los parámetros del problema
void Mesh(const char *filename, double **nodos, int** elementos, unsigned int* Materials, 
int dim, int NNodes, int NElements, int NNodes_Elemento) {
    FILE *file = fopen(filename, "r");
    char line[256];

    if (file == NULL) {
        printf("No se pudo abrir el archivo '%s'\n", filename);
        return;
    }

    unsigned int readingCoordinates = 0, readingElements = 0, nodeCount = 0, elementCount = 0;
    unsigned int elementPos, CoordPos;

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
        }

        if (readingCoordinates == 1 && nodeCount == 0) {
            // printf("%s", line);
            // Lectura de vector de coordenadas
            for (int i = 0; i < NNodes; i++) {
                
                // Posición de la coordenada
                if (fscanf(file, "%d", &CoordPos) != 1) { 
                    fclose(file);
                    return; // Error de lectura
                }

                // printf("Posición coordenada: %d\n", CoordPos);

                // Lee el valor del nodo y lo guarda
                // if (fscanf(file, "%lf", &nodos[i]) != 1) {
                //     fclose(file);
                //     return;  // Error de lectura
                // }

                // Caso 1d
                if (dim == 1){ 
                    for (int j = 0; j < dim; j++){
                        if (fscanf(file, "%lf", &nodos[i][j]) != 1) { // Asignar valores directamente a la matriz
                            fclose(file);
                            return; // Error de lectura
                        }

                        // Salta segundo valor nulo
                        fscanf(file, "%*f");

                        // printf("%lf\n", nodos[i][j]);

                    }

                }

                // Caso 2d en adelante
                if (dim != 1){ 
                    for (int j = 0; j < dim; j++){
                        if (fscanf(file, "%lf", &nodos[i][j]) != 1) { 
                            fclose(file);
                            return; // Error de lectura
                        }

                        // printf("%lf\n", nodos[i][j]);

                    }

                }
                
            }

            nodeCount ++;

        } else if (readingElements == 1 && elementCount == 0) {
            // printf("%s", line);
            fgets(line, sizeof(line), file); // Información sobre elemento
            // printf("Leído: %s", line);
            // Lectura de matriz de conexiones
            for (int i = 0; i < NElements; i++)
            {
                // Posición del elemento
                if (fscanf(file, "%d", &elementPos) != 1) { 
                    fclose(file);
                    return; // Error de lectura
                }

                // printf("Posición elemento: %d\n", elementPos);

                // Leer material
                if (fscanf(file, "%d", &Materials[elementPos - 1]) != 1) { 
                    fclose(file);
                    return; // Error de lectura
                }

                // printf("Material : %d\n", Materials[elementPos-1]);

                for (int j = 0; j < NNodes_Elemento; j++)
                {
                    // Asignar valores directamente a la matriz
                    if (fscanf(file, "%d", &elementos[elementPos - 1][j]) != 1) {
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

// Función que lee las propiedades del material
void ReadMaterial(const char *filename, double* D, double* Q,int NMaterials){
    FILE *file = fopen(filename, "r");
    char line[256];

    if (file == NULL) {
        printf("No se pudo abrir el archivo '%s'\n", filename);
        return;
    }

    unsigned int readingMat = 0;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        
        // printf("%s", line);

        if (strstr(line, "End Materials")) {
            readingMat = 0;
            continue;
        }

        if (strstr(line, "Materials")) {
            readingMat = 1;
        }

        if (readingMat == 1) {

            fgets(line, sizeof(line), file); // Información sobre el material

            // Leer las propiedades de los materiales
            for (int i = 0; i < NMaterials; i++) {

                // Salta el número del material
                fscanf(file, "%*d");

                // Leer el valor de la conductividad térmica
                if (fscanf(file, "%lf", &D[i]) != 1) { 
                    fclose(file);
                    return; // Error de lectura
                }
                // Leer el valor de la fuente de masa
                if (fscanf(file, "%lf", &Q[i]) != 1) { 
                    fclose(file);
                    return; // Error de lectura
                }

                
            }

        }

    }
    fclose(file);
}

// Función que escribe los resultados en un archivo .post.res
void WriteResults(const char *filename, double *Phi, double **q, double **q_pg, int NNodes, int NElements, int dim) {

    // Encontrar la última ocurrencia del punto en el nombre del archivo
    char *dotPosition = strrchr(filename, '.');
    if (dotPosition == NULL) {
        printf("Error: No se encontró una extensión de archivo válida.\n");
        return;
    }

    // Calcular la longitud del nombre base del archivo (sin la extensión)
    int baseNameLength = dotPosition - filename;

    // Preparar el nuevo nombre del archivo con la nueva extensión
    char *nameRes = (char *)malloc(baseNameLength + 10); // Asumiendo que ".post.res" + NULL cabe
    if (nameRes == NULL) {
        printf("Error: No se pudo asignar memoria para el nuevo nombre del archivo.\n");
        return;
    }

    // Copiar la parte base del nombre y agregar la nueva extensión
    snprintf(nameRes, baseNameLength + 10, "%.*s.post.res", baseNameLength, filename);

    // Continuar con la creación del archivo y la escritura de resultados...
    FILE *file = fopen(nameRes, "w");

    if (file == NULL) {
        printf("Error opening file!\n");
        free(nameRes);
        return;
    }

    // Escribir el encabezado del archivo .post.res
    fprintf(file, "GiD Post Results File 1.0\n\n");

    // Escribir los resultados de temperatura
    fprintf(file, "Result \"Temperature\" \"Load Case 1\" 1 Scalar OnNodes\n");
    fprintf(file, "ComponentNames \"T\"\n");
    fprintf(file, "Values\n");

    for (int i = 1; i <= NNodes; ++i) {
        fprintf(file, "%d %lf\n", i, Phi[i-1]);
    }

    fprintf(file, "End Values\n\n");

    // Asignar valores por defecto para la componente z en caso de que dim sea 2
    double default_z = 0.0;

    // Resultados de flujos en elementos en puntos de Gauss
    fprintf(file, "Result \"Element_Fluxes\" \"Load Case 1\" 1 Vector OnGaussPoints \"GP_ELEMENT_1\"\n");
    fprintf(file, "ComponentNames \"Flux_x\", \"Flux_y\", \"Flux_z\"\n");
    fprintf(file, "Values\n");

    for (int i = 0; i < NElements; i++) {  // Asumiendo que los índices de elementos empiezan en 0
        // Escribe dos componentes si dim es 2, y tres si dim es 3
        if (dim == 2) {
            fprintf(file, "%d %lf %lf %lf\n", i + 1, q_pg[i][0], q_pg[i][1], default_z);
        } else if (dim == 3) {
            fprintf(file, "%d %lf %lf %lf\n", i + 1, q_pg[i][0], q_pg[i][1], q_pg[i][2]);
        }
    }

    fprintf(file, "End Values\n\n");

    // Resultados de flujos promedio en nodos
    fprintf(file, "Result \"Average Flow\" \"Load Case 1\" 1 Vector OnNodes\n");
    fprintf(file, "ComponentNames \"Flux_x\", \"Flux_y\", \"Flux_z\"\n");
    fprintf(file, "Values\n");

    for (int i = 0; i < NNodes; i++) {  // Asumiendo que los índices de nodos empiezan en 0
        // Escribe dos componentes si dim es 2, y tres si dim es 3
        if (dim == 2) {
            fprintf(file, "%d %lf %lf %lf\n", i + 1, q[i][0], q[i][1], default_z);
        } else if (dim == 3) {
            fprintf(file, "%d %lf %lf %lf\n", i + 1, q[i][0], q[i][1], q[i][2]);
        }
    }

    fprintf(file, "End Values\n");

    // Liberar memoria
    free(nameRes);

    // Cerrar el archivo
    fclose(file);


}

// Función que escribe la malla
void WriteMesh(const char *filename, int** elementos, double **nodos, int NNodes, int NElements, 
int NNodes_Elemento, int dim, const char *ElementType) {

    // Encontrar la última ocurrencia del punto en el nombre del archivo
    char *dotPosition = strrchr(filename, '.');
    if (dotPosition == NULL) {
        printf("Error: No se encontró una extensión de archivo válida.\n");
        return;
    }

    // Calcular la longitud del nombre base del archivo (sin la extensión)
    int baseNameLength = dotPosition - filename;

    // Preparar el nuevo nombre del archivo con la nueva extensión
    char *nameRes = (char *)malloc(baseNameLength + 5); // ".msh" + NULL
    if (nameRes == NULL) {
        printf("Error: No se pudo asignar memoria para el nuevo nombre del archivo.\n");
        return;
    }

    // Copiar la parte base del nombre y agregar la nueva extensión
    snprintf(nameRes, baseNameLength + 5, "%.*s.msh", baseNameLength, filename);

    // Abrir el archivo para escritura
    FILE *file = fopen(nameRes, "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        free(nameRes);
        return;
    }

    // Escribir el encabezado del archivo
    fprintf(file, "MESH dimension %d ElemType %s Nnode %d\n", dim, ElementType, NNodes_Elemento);
    fprintf(file, "Coordinates\n");

    // Escribir las coordenadas de los nodos
    for (int i = 0; i < NNodes; i++) {
        fprintf(file, "%5d", i + 1); // Los índices de nodo comienzan en 1
        for (int j = 0; j < dim; j++) {
            fprintf(file, " %15.8e", nodos[i][j]);
        }
        fprintf(file, "\n");
    }

    fprintf(file, "End Coordinates\n\n");
    fprintf(file, "Elements\n");

    // Escribir los elementos
    for (int i = 0; i < NElements; i++) {
        fprintf(file, "%5d", i + 1); // Los índices de elemento comienzan en 1
        for (int j = 0; j < NNodes_Elemento; j++) {
            fprintf(file, " %5d", elementos[i][j]);
        }
        fprintf(file, "\n");
    }

    fprintf(file, "End Elements\n");

    // Cerrar el archivo y liberar memoria
    fclose(file);
    free(nameRes);
}