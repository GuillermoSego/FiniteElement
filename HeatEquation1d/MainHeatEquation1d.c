
/*
Programa para resolver el método de elementos finitos en una dimensión. Trabajamos con una malla
equidistante (esto puede cambiar). Con un número de nodos fijos por cada elemento. Este programa
no puede trabajar con mallas conformadas por diferentes elementos.

Guillermo Segura Gómez
Centro de Investigación en Matemáticas
Marzo 2024
*/

# include <stdio.h>
# include <stdlib.h>
# include "matrix.h"
# include "finiteElement.h"

int main(int argc, char *argv[]){

    // Validación de número de argumentos pasados por la línea de comandos
    if(argc != 2) {
        printf("Uso: %s <nombre_archivo>\n", argv[0]);
        return 1;
    }

    // Asignación del nombre del archivo a una variable constante
    const char* filename_dat = argv[1];
    
    #pragma region Paso 1. Formulación del problema.

    /*
    Necesitamos escanear los archivos de la malla generada y definir el problema.
    Utilizamos memoria dinámica para trabajar con los vectores que no conocemos. 

    Paso 1. Formulación del problema.
    En este caso trabajamos con la ecuación de calor. Para aplicar el método de los elementos 
    finitos, primero necesitamos establecer condiciones iniciales y de borde para la ecuación.
    */

    // Declaración de variables del sistema
    // int NElements = 9; // Número de elementos es siempre NNodes - 1 para una malla 1D
    // int NNodes = NElements + 1; // Número de nodos
    // int dim = 1;
    // int NNodes_Elemento = 2; // Nodos por elemento

    int dim, NNodes, NElements, NNodes_Elemento, NMaterials, NCDirichlet, NCNeumann;
    char* ElementType = NULL;

    ProblemDef(filename_dat, &dim, &NNodes, &NElements, &NMaterials, &ElementType, &NNodes_Elemento, 
    &NCDirichlet, &NCNeumann);

    // printf("Tipo de elemento: %s", ElementType);

    double** nodos = createMatrix(NNodes, dim); // Matriz para almacenar las coordendas de los nodos
    int** elementos = malloc( NElements * sizeof(int *)); // Matriz para almacenar la conexión entre nodos para cada elemento
    unsigned int* Materials = malloc( NElements * sizeof(int)); // Matriz para almacenar los materiales

    for (int i = 0; i < NElements; i++) {
        // Asigna memoria para cada columna de esta fila
        elementos[i] = malloc(NNodes_Elemento * sizeof(int));
    }


    #pragma endregion

    #pragma region Paso 2. Discretización del dominio.

    /*
    Paso 2. Discretización del dominio. Generación de la Malla
    Se escanean los archivos generados por GID.
    */

    // Leemos la malla
    Mesh(filename_dat, nodos, elementos, Materials, dim, NNodes, NElements, NNodes_Elemento);

    // Leemos la matriz de nodos
    // if (ReadVector(filename_Nodes, nodos, NNodes) == 1) {
    //     free(nodos); // Liberamos memoria
    //     return 0;
    // }

    // // Leemos la matriz de conexiones
    // if (ReadMatrix(filename_Connections, elementos, NElements, NNodes_Elemento) == 1) {
    //     free(elementos); // Liberamos memoria
    //     return 0;
    // }


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

    // La estructura de la matriz de conectividades es elemento[Número de elemento][Número de nodo]
    
    #pragma endregion

    #pragma region Paso 3. Funciones de forma.

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
    double **NEval = createMatrix(dim, NNodes_Elemento);
    for (int i = 0; i < NNodes_Elemento; i++) {
        NEval[0][i] = N[i](xi); 
    }

    double **NEvalT = createMatrix(NNodes_Elemento, dim);
    MatrixT(dim, NNodes_Elemento, NEval, NEvalT);

    // Vector para almacenar los valores de las derivadas evaluadas en los puntos de Gauss
    double **DNDE = createMatrix(dim, NNodes_Elemento);

    for (int i = 0; i < NNodes_Elemento; i++) {
        DNDE[0][i] = dN_dxi[i](xi); // Calcular y almacenar las derivadas
    }

    // printf("Las derivadas de las funciones de forma \n");
    // for (int i = 0; i < NNodes_Elemento; i++){
    //     printf("%lf \n", DNDE[0][i]);
    // }

    #pragma endregion

    #pragma region Paso 4 y 5. Construcción de matrices y ensamble 

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

    // Definimos el vector solución Phi
    double* Phi = calloc( NNodes, sizeof(double));

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

        x1 = nodos[conect[0] - 1][0];
        x2 = nodos[conect[1] - 1][0];
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

    // printf("Matrix de rigidez\n");
    // MatrixShow(NNodes, NNodes, K);

    // printf("Vector de fuerzas\n");
    // VectorShow(NNodes, 1, F);

    #pragma endregion

    #pragma region Paso 6. Aplicación de las condiciones de contorno

    /*
    Paso 6. Aplicación de las condiciones de contorno
    */

    // Leemos las condiciones de Dirichlet y Neumann
    BoundaryConditions *bc = readConditions(filename_dat, NCDirichlet, NCNeumann);

    // Mostrar las condiciones
    // Comprobar si se devolvió correctamente la estructura BoundaryConditions
    // if (bc != NULL) {
    //     // La función se ejecutó correctamente, puedes usar los datos de 'bc' aquí

    //     // Ejemplo de acceso a los datos:
    //     printf("Condiciones Dirichlet:\n");
    //     for (int i = 0; i < NCDirichlet; i++) {
    //         printf("Nodo: %d, Valor: %f\n", bc->dirichletNodes[i], bc->dirichletValues[i]);
    //     }

    //     // No olvides liberar la memoria asignada dinámicamente
    //     free(bc->dirichletNodes);
    //     free(bc->dirichletValues);
    //     free(bc->neumannNodes); // Incluso si NCNeumann es 0, es buena práctica intentar liberar, ya que free(NULL) es seguro
    //     free(bc->neumannValues);
    //     free(bc);
    // } else {
    //     printf("Error al leer las condiciones del archivo.\n");
    // }

    // Aplicación de las condiciones de contorno
    if (bc != NULL) {
        // Aplicación de condiciones de Dirichlet
        for (int i = 0; i < NCDirichlet; i++) {
            int index = bc->dirichletNodes[i] - 1; // Usa el nodo específico para la condición de Dirichlet
            for (int j = 0; j < NNodes; j++) {
                K[index][j] = 0.0;  // Poner la fila correspondiente de la matriz K en 0
                K[j][index] = 0.0;  // Poner la columna correspondiente de la matriz K en 0
            }
            K[index][index] = 1.0;  // Establecer la diagonal en 1 para mantener la solución
            F[index] = bc->dirichletValues[i];  // Establecer el valor en el vector de solución
        }

        // Aplicación de condiciones de Neumann
        for (int i = 0; i < NCNeumann; i++) {
            int index = bc->neumannNodes[i] - 1; // Usa el nodo específico para la condición de Neumann
            // Aplicar el valor de la condición de Neumann al vector F en el nodo específico
            F[index] += bc->neumannValues[i]; // Sumar el valor de Neumann
        }
    }

    // printf("Matrix de rigidez\n");
    // MatrixShow(NNodes, NNodes, K);

    // printf("Vector solución\n");
    // VectorShow(NNodes, 1, Phi);

    // printf("Vector de fuerzas\n");
    // VectorShow(NNodes, 1, F);

    #pragma endregion

    #pragma region Paso 7. Solución sistema K Phi = F

    /*
    Paso 7. Solución
    Utilizamos un método numérico para resolver el sistema K phi = F. Es un clásico
    problema Ax = b. Podemos utilizar métodos directos como iterativos. Al tener matrices
    sparse, es mejor utilizar métodos iterativos por el costo computacional. 
    */

    // Necesitamos aplanar la matriz
    double *KFlat = malloc(NNodes * NNodes * sizeof(double));
    FlattenMatrix(K, NNodes, NNodes, KFlat);

    // Método del gradiente conjugado
    Conjugate_gradient(KFlat, F, Phi, NNodes, NNodes);

    // Método de Cholesky
    // double* L = calloc(NNodes, sizeof(double));

    // if (cholesky(KFlat, L, NNodes) == -1) {
    //     printf("Error durante la factorización\n");
    //     return -1;
    // }

    // solveCholesky(L, F, Phi, NNodes);

    // Checar si tenemos solución
    // double tolerance = 1e-3;  // Define umbral de tolerancia
    // int control = isSolution(KFlat, Phi, F, NNodes, tolerance);

    // if (control == 0 ) {
    //     printf("La solución encontrada es correcta\n");
    // } else {
    //     printf("La solución encontrada no es correcta.\n");
    // }

    // printf("La solución del sistema\n");
    // VectorShow(NNodes, 1, Phi);

    #pragma endregion

    #pragma region Paso 8. Cálculo de los flujos q en los nodos

    /*
    Paso 8. Cálculo de los flujos q en los nodos
    Calculamos los flujos en cada nodo. Para esto primero tenemos que calcular los flujos 
    en los puntos de Gauss. Con eso ensamblamos la matriz de masa M elemental y el vector
    p elemental de promedios. Resolvemos y encontramos los flujos en los nodos. Es un pro-
    ceso análogo al realizado cuando resolvimos el sistema por elementos finitos.
    */

    
    // Inicializamos M y M elemental
    double** MElemental = createMatrix(NNodes_Elemento, NNodes_Elemento);
    double** M = createMatrix(NNodes, NNodes);
    Matrix_Initialize(M, NNodes); // Inicializamos a cero

    // Inicializamos P y P elemental
    double* PElemental = malloc( NNodes_Elemento * sizeof(double));
    double* P = calloc( NNodes, sizeof(double));

    // Phi elemental
    double* PhiElemental = malloc(NNodes_Elemento * sizeof(double));

    // q en los puntos de gauss
    double* q_gp = malloc(dim * sizeof(double));
    double* BFlat = malloc(NNodes_Elemento * dim * sizeof(double));
    double* NEvalTFlat = malloc(NNodes_Elemento * dim * sizeof(double));

    // q en los nodos
    double* q = calloc( NNodes, sizeof(double));

    // Nuevamente iteramos sobre el total de los elementos
    for (int k = 0; k<NElements; k++){

        // Nos movemos en cada elemento y cada nodo calculando MElemental         
        for(int w = 0; w<NNodes_Elemento; w++){
            conect[w] = elementos[k][w]; // Encontrar las conectividades
        }

        // Coordenadas de los nodos 
        x1 = nodos[conect[0] - 1][0];
        x2 = nodos[conect[1] - 1][0];
        // printf("%lf, %lf\n", x1, x2);

        // Calculo de la matriz M elemental
        MatrixProduct(NEvalT, NEval, MElemental, NNodes_Elemento, dim, NNodes_Elemento);

        // Multiplicación por el determinante del jacobiano
        MatriXEscalar(MElemental, detJacobian1d(x1, x2), NNodes_Elemento, NNodes_Elemento);

        // printf("Iteración %d\n", k);
        // MatrixShow(NNodes_Elemento, NNodes_Elemento, MElemental);

        // printf("Matriz elemental \n");
        // Ensamblaje de la matriz de masa
        for(int i = 0; i<NNodes_Elemento; i++){
            for(int j = 0; j<NNodes_Elemento; j++){
                // printf("Valores conect %d, %d\n", conect[i] - 1, conect[j] - 1);
                M[conect[i] - 1][conect[j] - 1] += MElemental[i][j];
            }
        }

        // Cálculo de q en los puntos de gauss

        // Matrix B
        BuildB1d(B, DNDE, x1, x2, dim, NNodes_Elemento);

        // Multiplicación por D
        MatriXEscalar(B, D, dim, NNodes_Elemento);

        // Construcción de Phi elemental
        for(int i = 0; i<NNodes_Elemento; i++){
            PhiElemental[i] = Phi[i + k];
        }

        // printf("Iteración %d\n", k);
        // printf("Phi Elemental\n");
        // VectorShow(NNodes_Elemento, 1, PhiElemental);

        // Calculo de p_gp
        FlattenMatrix(B, dim, NNodes_Elemento, BFlat);

        // printf("B\n");
        // VectorShow(dim, NNodes_Elemento, BFlat);

        VectorProduct(BFlat, PhiElemental, q_gp, dim, NNodes_Elemento, 1);

        // printf("Iteración %d\n", k);
        // printf("Q_pg\n");
        // VectorShow(dim, 1, q_gp);

        // Construcción de P elemental
        FlattenMatrix(NEvalT, NNodes_Elemento, dim, NEvalTFlat);
        VectorProduct(NEvalTFlat, q_gp, PElemental, NNodes_Elemento, dim, 1);
        // Multiplicamos por el jacobiano
        Divide(PElemental, 1.0/detJacobian1d(x1, x2), PElemental, NNodes_Elemento);
        
        // Ensamble de P
        for(int i = 0; i<NNodes_Elemento; i++){
            P[i + k] += PElemental[i];
        }
        
    }

    // printf("Matrix de masa\n");
    // MatrixShow(NNodes, NNodes, M);

    // printf("Vector de promedios\n");
    // VectorShow(NNodes, 1, P);  

    #pragma endregion

    #pragma region Paso 8.2 Solución sistema M q = P

    // Aplanar la matriz
    double *MFlat = malloc(NNodes * NNodes * sizeof(double));
    FlattenMatrix(M, NNodes, NNodes, MFlat);

    // Método del gradiente conjugado
    Conjugate_gradient(MFlat, P, q, NNodes, NNodes);

    // Checar si tenemos solución
    // double tolerance = 1e-3;
    // int control = isSolution(MFlat, q, P, NNodes, tolerance);

    // if (control == 0 ) {
    //     printf("La solución de flujos encontrada es correcta\n");
    // } else {
    //     printf("La solución de flujo encontrada no es correcta.\n");
    // }

    // printf("La solución del sistema q\n");
    // VectorShow(NNodes, 1, q);

    #pragma endregion

    #pragma region Escribir solución res

    // Escribimos la solución en un archivo
    WriteResults(filename_dat, Phi, q, NNodes, dim);
    
    // Escribimos la malla
    WriteMesh(filename_dat, elementos, nodos, NNodes, NElements, NNodes_Elemento, dim, ElementType);

    #pragma endregion

    #pragma region Liberar memoria

    // Liberar la memoria
    if (ElementType != NULL) {
        free(ElementType);
    }
    for (int i = 0; i < NElements; i++) {
        free(elementos[i]); // Libera cada fila de la matriz
    }
    free(elementos); // Libera el arreglo de filas
    freeMatrix(nodos, NNodes); 
    free(Materials);
    freeMatrix(NEval, dim);
    freeMatrix(NEvalT, NNodes_Elemento);
    freeMatrix(DNDE, dim);
    freeMatrix(K, NNodes);
    freeMatrix(B, dim);
    freeMatrix(BT, NNodes_Elemento);
    freeMatrix(KElemental, NNodes_Elemento);
    free(conect);
    free(FElemental);
    free(F);
    free(Phi);

    free(bc->dirichletNodes);
    free(bc->dirichletValues);
    free(bc->neumannNodes);
    free(bc->neumannValues);
    free(bc);

    free(KFlat);
    
    // free(L);

    freeMatrix(MElemental, NNodes_Elemento);
    freeMatrix(M, NNodes);
    free(PElemental);
    free(BFlat);
    free(NEvalTFlat);
    free(P);
    free(PhiElemental);
    free(q_gp);
    free(q);
    free(MFlat);

    #pragma endregion

    return 0;
}