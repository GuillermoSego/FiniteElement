
/*
Programa para resolver el método de elementos finitos en dos dimensiones. Este programa
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
    Paso 1. Formulación del problema.
    */

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
    double (*N[NNodes_Elemento])(double, double);

    // Asignar las funciones al vector
    N[0] = N1;
    N[1] = N2;
    N[2] = N3;

    // Vector para almacenar las funciones de derivadas de las funciones de forma
    double (*dN_dxi[NNodes_Elemento])(double, double);

    // Asignar las funciones de derivadas al vector
    dN_dxi[0] = dN1de;
    dN_dxi[1] = dN2de;
    dN_dxi[2] = dN3de;
    dN_dxi[3] = dN1dn;
    dN_dxi[4] = dN2dn;
    dN_dxi[5] = dN3dn;


    double xi = 1.0/3.0; // Punto de Gauss

    // Evaluar las funciones en los puntos de gauss
    double **NEval = createMatrix(1, NNodes_Elemento);
    for (int i = 0; i < NNodes_Elemento; i++) {
        NEval[0][i] = N[i](xi, xi); 
    }

    double **NEvalT = createMatrix(NNodes_Elemento, 1);
    MatrixT(1, NNodes_Elemento, NEval, NEvalT);

    // Vector para almacenar los valores de las derivadas evaluadas en los puntos de Gauss
    double **DNDE = createMatrix(dim, NNodes_Elemento);

    for(int j = 0; j<dim; j++){    
        for (int i = 0; i < NNodes_Elemento; i++) {
            DNDE[j][i] = dN_dxi[i+3*j](xi, xi); // Calcular y almacenar las derivadas
        }
    }

    // printf("Las derivadas de las funciones de forma \n");
    // MatrixShow(dim, NNodes_Elemento, DNDE);

    #pragma endregion

    #pragma region Paso 4 y 5. Construcción de matrices y ensamble 

    /*
    Paso 4. Transformamos la ecuación diferencial en su forma débil. 
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

    // Inicializamos D material, es un arreglo con el valor de la conductividad térmica en c/mat
    double* D = malloc(NMaterials * sizeof(double));
    double* Q = malloc(NMaterials * sizeof(double)); // Calor de masa

    // Leemos el material
    ReadMaterial(filename_dat, D, Q, NMaterials);

    // printf("Valores de la Q para %d materiales\n", NMaterials);
    // for(int i = 0; i<NMaterials; i++){
    //     printf("Material %d: Q: %lf\n", i+1, Q[i]);
    // }

    // Definimos la matriz jacobiana
    double **J = createMatrix(dim, dim);
    double **Jinv = createMatrix(dim, dim);

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
    // double Q = 5.0;

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

   // Definimos una matriz para las coordenadas
   double **coord = createMatrix(dim, NNodes_Elemento);
   double detJ = 0.0;

   double wi = 2.0; // Pesos de Gauss

    // Iteramos sobre el total de los elementos
    for (int k = 0; k<NElements; k++){

        // Nos movemos en cada elemento y cada nodo calculando KElemental         
        for(int w = 0; w<NNodes_Elemento; w++){
            conect[w] = elementos[k][w];
        }

        // Coordenadas del nodo
        for(int a = 0; a<dim; a++){
            for(int b = 0; b<NNodes_Elemento; b++){
                coord[a][b] = nodos[conect[b] - 1][a];
            }
        }

        // printf("Iteración %d\n", k);
        // MatrixShow(dim, NNodes_Elemento, coord);

        // Construimos la matriz Jacobiana
        BuildJacobian(J, DNDE, coord, dim, NNodes_Elemento);

        // printf("Jacobiano\n");
        // MatrixShow(dim, dim, J);

        // Calculamos el determinante del jacobiano
        detJ = DetJacobian(J, dim);

        // Calculamos la inversa del jacobiano
        InvJacobian(J, Jinv, detJ, dim);

        // Calculo de la matriz B = Jinv*DNDE
        MatrixProduct(Jinv, DNDE, B, dim, dim, NNodes_Elemento);
        
        // Calculo matriz B^T
        MatrixT(dim, NNodes_Elemento, B, BT);

        // printf("Iteración %d\n", k);
        // MatrixShow(dim, NNodes_Elemento, B);
        // MatrixShow(NNodes_Elemento, dim, BT);

        // Multiplicación por D
        MatriXEscalar(B, D[Materials[k] - 1], dim, NNodes_Elemento);

        // Cálculo de KElemental = \int B^T D B
        MatrixProduct(BT, B, KElemental, NNodes_Elemento, dim, NNodes_Elemento);
        
        // Multiplicación por el determinante del jacobiano y los pesos
        MatriXEscalar(K, detJ*wi, NNodes_Elemento, NNodes_Elemento);

        /*
        Si queremos trabajar con un elemento con mas puntos de Gauss tenemos
        que hacer un ciclo por cada punto y cada uno contribuye a la matriz K
        */
    
        // for (int pg = 0; pg < 3; pg++) {  // Asumiendo 3 puntos de Gauss
        //     double xi = GaussPoints[pg].xi;
        //     double eta = GaussPoints[pg].eta;
        //     double weight = GaussPoints[pg].weight;

        //     // Evaluar DNDE, construir J y Jinv, y calcular detJ para este punto de Gauss
        //     // ...

        //     // Calcular B y BT para este punto de Gauss
        //     // ...

        //     // Calcular la contribución de este punto de Gauss a KElemental
        //     for (int i = 0; i < NNodes_Elemento; i++) {
        //         for (int j = 0; j < NNodes_Elemento; j++) {
        //             for (int m = 0; m < dim; m++) {
        //                 KElemental[i][j] += BT[m][i] * D * B[m][j] * detJ * weight;
        //             }
        //         }
        //     }

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


        // Construcción F elemental = N^T * Q * detJ
        BuildF(FElemental, NEval, Q[Materials[k] - 1], detJ, NNodes_Elemento);
        // printf("Vector F elemental \n");
        // VectorShow(NNodes_Elemento, 1, FElemental);

        // printf("Longitud: %lf", -(x1 - x2));
    
        // Ensamble de F
        for(int i = 0; i<NNodes_Elemento; i++){
            F[conect[i] - 1] += FElemental[i];
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
    double* P = calloc( NNodes * dim, sizeof(double));

    // Phi elemental
    double* PhiElemental = malloc(NNodes_Elemento * sizeof(double));

    // q en los puntos de gauss
    double* q_gp = malloc(dim * sizeof(double));
    double* BFlat = malloc(NNodes_Elemento * dim * sizeof(double));

    // q en los nodos
    double* q = calloc( NNodes * dim, sizeof(double));

    // Nuevamente iteramos sobre el total de los elementos
    for (int k = 0; k<NElements; k++){

        // Nos movemos en cada elemento y cada nodo calculando MElemental         
        for(int w = 0; w<NNodes_Elemento; w++){
            conect[w] = elementos[k][w]; // Encontrar las conectividades
        }

        // Coordenadas del nodo
        for(int a = 0; a<dim; a++){
            for(int b = 0; b<NNodes_Elemento; b++){
                coord[a][b] = nodos[conect[b] - 1][a];
            }
        }


        // Construimos la matriz Jacobiana
        BuildJacobian(J, DNDE, coord, dim, NNodes_Elemento);

        // printf("Jacobiano\n");
        // MatrixShow(dim, dim, J);

        // Calculamos el determinante del jacobiano
        detJ = DetJacobian(J, dim);

        // Calculo de la matriz M elemental
        MatrixProduct(NEvalT, NEval, MElemental, NNodes_Elemento, 1, NNodes_Elemento);

        // Multiplicación por el determinante del jacobiano
        MatriXEscalar(MElemental, detJ*wi, NNodes_Elemento, NNodes_Elemento);

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

        // Calculamos la inversa del jacobiano
        InvJacobian(J, Jinv, detJ, dim);

        // Calculo de la matriz B = Jinv*DNDE
        MatrixProduct(Jinv, DNDE, B, dim, dim, NNodes_Elemento);

        // Multiplicación por D
        MatriXEscalar(B, D[Materials[k] - 1], dim, NNodes_Elemento);

        // Construcción de Phi elemental
        for(int i = 0; i<NNodes_Elemento; i++){
            PhiElemental[i] = Phi[conect[i] - 1];
        }

        // printf("Iteración %d\n", k);
        // printf("Phi Elemental\n");
        // VectorShow(NNodes_Elemento, 1, PhiElemental);

        // Calculo de q_pg
        FlattenMatrix(B, dim, NNodes_Elemento, BFlat);

        // printf("B\n");
        // VectorShow(dim, NNodes_Elemento, BFlat);

        VectorProduct(BFlat, PhiElemental, q_gp, dim, NNodes_Elemento, 1);

        // printf("Iteración %d\n", k);
        // printf("Q_pg\n");
        // VectorShow(dim, 1, q_gp);

        // Construcción de P elemental
        for(int j = 0; j < dim; j++){


            // Aplanamos en P Elemental            
            FlattenMatrix(NEvalT, NNodes_Elemento, 1, PElemental);

            // Calculamos el producto por cada dimension
            VectorXEscalar(PElemental, detJ*q_gp[j], PElemental, NNodes_Elemento);

            // printf("Iteración %d, dimensión %d\n", k, j);
            // VectorShow(NNodes_Elemento, 1, PElemental);

            // Ensamble de P por dimension
            for(int i = 0; i<NNodes_Elemento; i++){
                P[(conect[i] - 1) + j*NNodes] += PElemental[i];
            }


        }
        
    }

    // printf("Matrix de masa\n");
    // MatrixShow(NNodes, NNodes, M);

    // printf("Vector de promedios\n");
    // VectorShow(NNodes, 2, P);  

    #pragma endregion

    #pragma region Paso 8.2 Solución sistema M q = P

    // Arreglos temporales para almacenar P y q
    double *PTemp = malloc( NNodes * sizeof(double));
    double *qTemp = malloc( NNodes * sizeof(double));

    // Aplanar la matriz
    double *MFlat = malloc(NNodes * NNodes * sizeof(double));
    FlattenMatrix(M, NNodes, NNodes, MFlat);

    // Resolver utilizando el método del gradiente conjugado
    for(int i = 0; i<dim; i++){

        for(int j = 0; j<NNodes; j++){
            // Copiamos en PTemp
            PTemp[j] = P[j + i*NNodes];
        }

        // Método del gradiente conjugado
        Conjugate_gradient(MFlat, PTemp, qTemp, NNodes, NNodes);

        for(int j = 0; j<NNodes; j++){
            // Copiamos la solución en q
            q[j + i*NNodes] = qTemp[j];
        }

    }


    // Checar si tenemos solución
    // double tolerance = 1e-3;
    // int control = isSolution(MFlat, q, P, NNodes, tolerance);

    // if (control == 0 ) {
    //     printf("La solución de flujos encontrada es correcta\n");
    // } else {
    //     printf("La solución de flujo encontrada no es correcta.\n");
    // }

    // printf("La solución del sistema q\n");
    // VectorShow(NNodes, 2, q);

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
    free(D);
    free(Q);
    freeMatrix(NEval, 1);
    freeMatrix(NEvalT, NNodes_Elemento);
    freeMatrix(DNDE, dim);
    freeMatrix(K, NNodes);
    freeMatrix(B, dim);
    freeMatrix(BT, NNodes_Elemento);
    freeMatrix(KElemental, NNodes_Elemento);
    freeMatrix(coord, dim);
    freeMatrix(J, dim);
    freeMatrix(Jinv, dim);
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
    free(P);
    free(PhiElemental);
    free(q_gp);
    free(q);
    free(MFlat);

    free(PTemp);
    free(qTemp);

    #pragma endregion


return 0;

}