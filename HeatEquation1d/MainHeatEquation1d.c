
/*
Programa para resolver el método de elementos finitos en una dimensión. Trabajamos con una malla
equidistante (esto puede cambiar). Con un número de nodos fijos por cada elemento. Este programa
no puede trabajar con mallas 
*/

# include <stdio.h>
# include <stdlib.h>
# include "matrix.h"
# include "finiteElement.h"

int main(int argc, char *argv[]){

    // Validación de número de argumentos pasados por la línea de comandos
    if(argc != 3) {
        printf("Uso: %s <nombre_archivo>\n", argv[0]);
        return 1;
    }

    // Asignación del nombre del archivo a una variable constante
    const char* filename_Nodes = argv[1];
    const char* filename_Connections = argv[2];
    
    /*
    Necesitamos escanear los archivos de la malla generada y definir el problema.
    Utilizamos memoria dinámica para trabajar con los vectores que no conocemos. 

    Paso 1. Formulación del problema.
    En este caso trabajamos con la ecuación de calor. Para aplicar el método de los elementos 
    finitos, primero necesitamos establecer condiciones iniciales y de borde para la ecuación.
    */

    // Declaración de variables del sistema
    int NElements = 4; // Número de elementos es siempre NNodes - 1 para una malla 1D
    int NNodes = NElements + 1; // Número de nodos
    int dim = 1;
    int NNodes_Elemento = 2; // Nodos por elemento

    double* nodos = malloc(NNodes * sizeof(double)); // Arreglo para almacenar las coordenadas de los nodos
    int** elementos = malloc( NElements * sizeof(int *)); // Matriz para almacenar la conexión entre nodos para cada elemento

    for (int i = 0; i < NElements; i++) {
        // Asigna memoria para cada columna de esta fila
        elementos[i] = malloc(NNodes_Elemento * sizeof(int));
    }

    /*
    Paso 2. Discretización del dominio. Generación de la Malla
    Se escanean los archivos generados por GID.
    */

    // Leemos la matriz de nodos
    if (ReadVector(filename_Nodes, nodos, NNodes) == 1) {
        free(nodos); // Liberamos memoria
        return 0;
    }

    // Leemos la matriz de conexiones
    if (ReadMatrix(filename_Connections, elementos, NElements, NNodes_Elemento) == 1) {
        free(elementos); // Liberamos memoria
        return 0;
    }


    // // Mostrar las coordenadas de los nodos
    // VectorShow(NNodes, 1, nodos);

    // // Mostrar las conexiones de los elementos
    // printf("Elementos y sus nodos conectados:\n");

    // for (int i = 0; i < NElements; i++) {
    //     printf("Elemento %d. Nodos: ", i);
    //     for (int j = 0; j < NNodes_Elemento; j++){     
    //         printf("%d   ", elementos[i][j]);     // Nodo inicial del elemento i
    //     }
    //     printf("\n");
    // }

    // La estructura de la matriz de conectividades es elemento[Número de elemento][Número de nodo]
    
    /*
    Paso 3. Funciones de forma.
    Para cada elemento, definimos funciones de forma (también llamadas funciones de interpolación) 
    que describen cómo varía la solución dentro del elemento. 
    */

    // Vector para almacenar las funciones de las funciones de forma
    double (*N[NNodes_Elemento])(double);

    // Asignar las funciones al vector
    N[0] = N1;
    N[1] = N2;

    // Vector para almacenar las funciones de derivadas de las funciones de forma
    double (*dN_dxi[NNodes_Elemento])(double);

    // Asignar las funciones de derivadas al vector
    dN_dxi[0] = dN1de;
    dN_dxi[1] = dN2de;

    double xi = 0.0; // Punto de Gauss

    // Evaluar las funciones en los puntos de gauss
    double* NEval = malloc(NNodes_Elemento*sizeof(double));
    for (int i = 0; i < NNodes_Elemento; i++) {
        NEval[i] = N[i](xi); 
    }

    // Vector para almacenar los valores de las derivadas evaluadas en los puntos de Gauss
    double* DNDE = malloc(NNodes_Elemento*sizeof(double));

    for (int i = 0; i < NNodes_Elemento; i++) {
        DNDE[i] = dN_dxi[i](xi); // Calcular y almacenar las derivadas
    }

    // printf("Las derivadas de las funciones de forma \n");
    // for (int i = 0; i < NNodes_Elemento; i++){
    //     printf("%lf \n", DNDE[i]);
    // }

    /*
    Paso 4. Transformamos la ecuación diferencial en su forma débil. Esto implica multiplicar 
    la ecuación diferencial por una función de prueba v, integrar sobre el dominio, y aplicar 
    la integración por partes para reducir el orden de las derivadas. Esto nos da una formulación 
    en términos de los valores de la solución en los nodos, más manejable para la solución numérica.
    */

    /*
    Cálculo de la matriz de rigidez K
    */

    // Inicializamos la matriz K de rigidez
    double** K = createMatrix(NNodes, NNodes);
    Matrix_Initialize(K, NNodes); // Inicializamos a cero

    // Inicializamos la matriz B
    double** B = createMatrix(dim, NNodes_Elemento);
    // Inicializamos B transpuesta
    double** BT = createMatrix(NNodes_Elemento, dim);

    /* 
    La matriz B esta formada por las derivadas de las funciones de forma con respecto 
    al espacio físico. Para calcular estas derivadas se utiliza la regla de la cadena. 
    Generalizando, es suficiente con multiplicar por la inversa matriz jacobiana, a las
    derivadas de las funciones de forma evaluadas en los puntos de Gauss.
    */

    // Inicializamos K elemental
    double** KElemental = createMatrix(NNodes_Elemento, NNodes_Elemento);

    // Inicializamos D material, en este caso un solo valor
    double D = 1.0;

    // Definimos la matriz jacobiana
    // double **J = createMatrix(dim, dim); // No es necesario, solo hay una dimensión

    /* 
    La parte derecha de la ecuación K PHI = F 
    El vector F es el llamado vector de fuerzas. En este caso constituye el calor que emana
    internamente de la barra. El procedimiento de construcción del vector F es análogo a la 
    construcción de la matriz K. 
    */

    // Definición del vector F elemental
    double* FElemental = malloc( NNodes_Elemento * sizeof(double));

    // Vector F
    double* F = calloc( NNodes, sizeof(double));

    // Valor de Q. Calor emanado por la barra
    double Q = 5.0;

    /*
    Paso 5. Ensamble
    Las expresiones obtenidas para cada elemento se ensamblan en un sistema de ecuaciones 
    algebraicas que relacionan los valores de la solución en todos los nodos del dominio.
    */

   int conect1, conect2; // Conexiones actuales
   int* conect = malloc(NNodes_Elemento*sizeof(int));
   double x1, x2; // Coordenadas actuales


    // Iteramos sobre el total de los elementos
    for (int k = 0; k<NElements; k++){

        // Nos movemos en cada elemento y cada nodo calculando KElemental         
        for(int w = 0; w<NNodes_Elemento; w++){
            conect[w] = elementos[k][w];
        }

        x1 = nodos[conect[0] - 1];
        x2 = nodos[conect[1] - 1];
        // printf("%lf, %lf\n", x1, x2);

        // Calculo de la matriz B
        BuildB1d(B, DNDE, x1, x2, dim, NNodes_Elemento);

        // Calculo matriz B^T
        MatrixT(dim, NNodes_Elemento, B, BT);

        // printf("Iteración %d\n", i);
        // MatrixShow(dim, NNodes_Elemento, B);
        // MatrixShow(NNodes_Elemento, dim, BT);

        // Multiplicación por D
        MatriXEscalar(B, D, dim, NNodes_Elemento);

        // Cálculo de KElemental = \int B^T D B
        MatrixProduct(BT, B, KElemental, NNodes_Elemento, dim, NNodes_Elemento);
        
        // printf("Iteración %d\n", k);

        // printf("Matriz K elemental \n");
        // MatrixShow(NNodes_Elemento, NNodes_Elemento, KElemental);

        // printf("Matriz elemental \n");
        // Ensamblaje de la matriz de rigidez
        for(int i = 0; i<NNodes_Elemento; i++){
            for(int j = 0; j<NNodes_Elemento; j++){
                // printf("Valores conect %d, %d\n", conect[i] - 1, conect[j] - 1);
                K[conect[i] - 1][conect[j] - 1] += KElemental[i][j];
            }
        }


        // Construcción F elemental
        BuildF1d(FElemental, NEval, Q, x1, x2, NNodes_Elemento);
        // printf("Vector F elemental \n");
        // VectorShow(NNodes_Elemento, 1, FElemental);

        // printf("Longitud: %lf", -(x1 - x2));
    
        // Ensamble de F
        for(int i = 0; i<NNodes_Elemento; i++){
            F[i + k] += FElemental[i];
        }
        

    }

    // Condiciones de Dirichlet y Neumann
    // BoundaryConditions *bc = readConditions("CondFrontera1d.txt");

    // if (bc != NULL) {
    //     for (int i = 0; i < 2; i++) {
    //         if (bc->dirichletValid[i]) {
    //             printf("Condición Dirichlet %d: %f\n", i, bc->dirichlet[i]);
    //         } else {
    //             printf("Condición Dirichlet %d: None\n", i);
    //         }
    //     }

    //     for (int i = 0; i < 2; i++) {
    //         if (bc->neumannValid[i]) {
    //             printf("Condición Neumann %d: %f\n", i, bc->neumann[i]);
    //         } else {
    //             printf("Condición Neumann %d: None\n", i);
    //         }
    //     }

    //     free(bc); // No olvides liberar la memoria asignada
    // }



    printf("Matrix de rigidez\n");
    MatrixShow(NNodes, NNodes, K);

    printf("Vector de fuerzas\n");
    VectorShow(NNodes, 1, F);

    // Liberar la memoria
    for (int i = 0; i < NElements; i++) {
        free(elementos[i]); // Libera cada fila de la matriz
    }
    free(elementos); // Libera el arreglo de filas
    free(nodos); 
    free(NEval);
    free(DNDE);
    freeMatrix(K, NNodes);
    freeMatrix(B, dim);
    freeMatrix(BT, NNodes_Elemento);
    freeMatrix(KElemental, NNodes_Elemento);
    free(conect);
    free(FElemental);
    free(F);

    return 0;
}