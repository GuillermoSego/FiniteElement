# include <stdio.h>
# include <stdlib.h>
# include "matrix.h"
# include "finiteElement.h"

int main(){
    
    /*
    Paso 1. Formulación del problema.
    En este caso trabajamos con la ecuación de calor. Para aplicar el método de los elementos 
    finitos, primero necesitamos establecer condiciones iniciales y de borde para la ecuación.
    */

    // Declaración de variables del sistema
    int NElements; // Número de elementos es siempre NNodes - 1 para una malla 1D

    // Punto inicial y final de la malla
    double xi, xif;
    int NNodes_Elemento; // Nodos por elemento

    // Solicitar al usuario el número de elementos
    printf("Ingrese el número de elementos (NElements): ");
    scanf("%d", &NElements);

    // Solicitar al usuario el punto inicial de la malla (xi)
    printf("Ingrese el punto inicial de la malla (xi): ");
    scanf("%lf", &xi);

    // Solicitar al usuario el punto final de la malla (xif)
    printf("Ingrese el punto final de la malla (xif): ");
    scanf("%lf", &xif);

    // Solicitar al usuario el número de nodos por elemento (NNodes_Elemento)
    printf("Ingrese el número de nodos por elemento (NNodes_Elemento): ");
    scanf("%d", &NNodes_Elemento);

    int NNodes = NElements + 1; // Número de nodos
    int dim = 1; // Dimensión del problema

    // Arreglo para almacenar las coordenadas de los nodos
    double* nodos = malloc(NNodes * sizeof(double)); 

    // Matriz para almacenar la conexión entre nodos para cada elemento
    int** elementos = malloc(NElements * sizeof(int*));
    for (int i = 0; i < NElements; i++) { // Corrección en el ciclo for
        elementos[i] = malloc(NNodes_Elemento * sizeof(int));
    }


    /*
    Paso 2. Discretización del dominio. Generación de la Malla
    Se divide el dominio de la solución en un número finito de subdominios o "elementos".
    Este paso es proporcionado por GID.
    */

    // Se genera la malla unidimensional
    MallaGeneratorEquidist(dim, NNodes, nodos, xi, xif);

    // Guardar las coordenadas de los nodos en un archivo
    saveVectorToFile(nodos, NNodes, 1, "nodos.txt");

    // Mostrar las coordenadas de los nodos
    VectorShow(NNodes, 1, nodos);

    // Generar las conexiones de los elementos
    for (int i = 0; i < NElements; i++) {
        for (int j = 0; j < NNodes_Elemento; j++){     
            elementos[i][j] = j + i + 1;     // Nodo inicial del elemento i
        }
    }

    // Mostrar las conexiones de los elementos
    printf("Elementos y sus nodos conectados:\n");

    for (int i = 0; i < NElements; i++) {
        printf("Elemento %d. Nodos: ", i);
        for (int j = 0; j < NNodes_Elemento; j++){     
            printf("%d   ", elementos[i][j]);     // Nodo inicial del elemento i
        }
        printf("\n");
    }

    // Guardar las conexiones de los elementos en un archivo
    saveMatrixToFile(elementos, NElements, NNodes_Elemento, "elementos.txt");

    return 0;

}