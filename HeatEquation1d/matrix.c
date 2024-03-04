/*

Biblioteca para manejo de matrices

*/

# include <stdio.h>
# include <math.h>
# include <stdlib.h>


// Esta rutina muestra una matriz de mxn
void MatrixShow(int m, int n, double **v){

    int i, j, k=0;
    
    for(i=0 ; i<m ; i++){
        
        for (j=0 ; j<n ; j ++){
            
            printf("%lf \t", v[i][j]);
        }
        
        printf("\n");
    }
    
    
}

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

// Esta rutina calcula la matriz transpuesta de una matriz y la guarda en otro
void MatrixT(int rows, int cols, double **matrix, double **transposed) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            transposed[j][i] = matrix[i][j];
        }
    }
}


// Esta rutina calcula la matriz transpuesta de un vector y la guarda en otro
void VectorT(int n, int m, double v[], double Vec[] )
{

  double* VecPtr = Vec;
  double* vPtr = v;

  int i = 0, j = 0;
 
  do {

    do {
        
       *VecPtr = *vPtr;

        VecPtr ++; 

        vPtr += m;

      j ++;
    } while (j<n);   

    vPtr = v + (i+1);

    j = 0;
    i ++;
  } while (i<m);


}

// Esta rutina calcula el producto punto entre dos vectores
double DotProd(double x[], double y[], int n)
{
    
    double* xPtr = x;
    double* yPtr = y;
    
    double sum = 0;
    int i=0;
    
    do {
        sum += *( xPtr++ ) * *(yPtr ++ );
        i ++;
    } while (i<n);
    
    return sum;
}

// Esta rutina hace el producto entre dos matrices
void MatrixProduct(double **m1, double **m2, double **result, int M, int N, int O) {
    // Inicializamos la matriz resultante con ceros
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < O; j++) {
            result[i][j] = 0;
        }
    }

    // Realizamos la multiplicación de matrices
    for (int i = 0; i < M; i++) { // Iterar sobre las filas de m1
        for (int j = 0; j < O; j++) { // Iterar sobre las columnas de m2
            for (int k = 0; k < N; k++) { // Iterar sobre las columnas de m1 y las filas de m2
                result[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
}


// Esta rutina hace el producto entre dos vectores
void VectorProduct(double m1[], double m2[], double Matrix[], int M, int N, int O)
{

  // Apuntadores a las matrices
  double* m1Ptr = m1;
  double* m2Ptr = m2;
    
  // Arreglo new, va guardar los elementos de la columna de la segunda matriz
  double new[N];
  double* nPtr = new;
    
  int i = 0, j = 0, k = 0;
    
  // Declaramos la matriz producto
  double* MPtr = Matrix;
    
  /*
    
  Para hacer la multiplicacion de matrices necesitamos tres ciclos, en el 
  primero, recorremos los elementos de la matriz 1. En el segundo recorremos
  las filas de la matriz 2. Y en el tercero las columnas. Vamos guardando las
  columnas de la matriz 2 en un arreglo llamado new y hacemos el producto 
  punto con los elementos de la fila de la matriz 1.
    
  */
    
  do {
        
      do {
            
          // Apuntamos en la direccion correcta
          nPtr = new;
          m2Ptr = m2;
          j = 0;
           
          do {
               
              // Guardamos la columna en el arreglo new
              *nPtr = *(m2Ptr + i + j*O);
              nPtr ++;
               
              j ++;
               
          } while (j<N);
            
          // Apuntamos en la direccion correcta
          nPtr = new;
            
          /*
            
          Hacemos el producto punto entre la fila de la matriz 1 y la columna 
          de la matriz 2
            
          */
          *MPtr = DotProd(m1Ptr, nPtr, N);
          MPtr ++;
            
          i ++;
            
      } while(i<O);
    
      i = 0;
      k ++;
        
      // Sumamos para estar en la siguiente fila
      m1Ptr = m1Ptr + N;
    
  } while(k<M);

}

// Rutina para leer una matriz
int ReadMatrix(const char *filename, int **A, int rows, int cols) {
    FILE *file;
    int i, j;

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return 1;
    }

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            if (fscanf(file, "%d", &A[i][j]) != 1) { // Asignar valores directamente a la matriz
                fclose(file);
                return 3; // Error de lectura
            }
        }
    }

    fclose(file);
    return 0;
}


// Rutina para leer un vector desde un archivo
int ReadVector(const char *filename, double *A, int n) {
    FILE *file;
    int i;

    // Abrimos el archivo en modo lectura
    file = fopen(filename, "r");

    // Comprobamos si el archivo se abrió con éxito
    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return 1;  // Error al abrir el archivo
    }

    // Leemos el archivo y asignamos los valores al vector A
    for (i = 0; i < n; i++) {
        if (fscanf(file, "%lf", &A[i]) != 1) {
            fclose(file);
            return 3;  // Error de lectura
        }
    }

    // Cerramos el archivo
    fclose(file);

    return 0;  // Éxito
}
// Este programa calcula el valor maxino de un arreglo
double maximus(double v[], int s)
{
    
    // Apuntamos
    double* vPtr = v;
    
    // Donde guardamos el minimo
    double m;
    
    int i = 0;
    
    // Guardamos el primer valor del arreglo en m
    m = *vPtr;
    
    do {
        
        // Comparamos
        if (*vPtr >= m) {
            
            m = *vPtr;
            
        }
        
        vPtr ++;
        i ++;
        
    } while(i< s);
    
    return m;
    
}

// Esta rutina inicializa a cero un arreglo
void Initialize(double in[], int n)
{

  double* inPtr = in;
  int i = 0;

  do {

    *inPtr = 0;
    inPtr ++;

    i ++;

  } while(i<n);

}

// Esta rutina calcula la suma de dos matrices cuadradas
void MatrixSum(double m1[], double m2[], double mout[], int n)
{

  double* m1Ptr = m1;
  double* m2Ptr = m2;
  double* outPtr = mout;

  int i = 0;

  do {

    *outPtr = *m1Ptr - *m2Ptr;
    m2Ptr ++;
    m1Ptr ++;
    outPtr ++;

    i ++;

  } while( i < (n*n) );

}

// Este programa suma todos los elementos de un vector, y los pone en otro vec
void Sum(double in[], double out[], int n)
{

  Initialize(out, n);

  double sum = 0;
  // Apuntamos
  double* inPtr = in;
  double* outPtr = out;
  int i = 0, j = 0;

  do{

    do {

      sum += *inPtr;
      inPtr ++;

      i ++;

    } while(i<n);

    *outPtr += sum; 
    outPtr ++;
    sum = 0;
    i = 0;
    j ++;

  } while(j<n);

}

// Esta rutina hace la multiplicación de una matriz cuadrada por un escalar
void MatriXEscalar(double** mat, double x, int rows, int cols){

    for(int i = 0; i<rows; i++){
        for(int j = 0; j<cols; j++){
            mat[i][j] = mat[i][j] * x;
        }
    }

}

// Este programa divide un arreglo por un escalar
void Divide(double matrix[], double scalar, double out[], int s)
{

  double* mPtr = matrix;
  double* outPtr = out;
  int i = 0;

  do {

    *outPtr = *mPtr/scalar;
    outPtr ++;
    mPtr ++;

    i ++;

  } while( i < s );

}

// Esta rutina inicializa un arreglo de unos
void Ones(double t[], int n)
{

  double* tPtr = t;

  int i = 0;

  do {

    *tPtr = 1;
    tPtr ++;

    i++;

  } while(i<n);


}

// Rutina para restar v2 de v1 (v1 = v1 - v2)
void SubtractVector(double *v1, double *v2, int n) {
    for (int i = 0; i < n; i++) {
        v1[i] -= v2[i];
    }
}

// Rutina para guardar un vector en un archivo
void saveVectorToFile(double *A, int n, int m, const char *filename) {
    FILE *file = fopen(filename, "w");
    
    if (file == NULL) {
        printf("Error al abrir el archivo.\n");
        return;
    }

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            fprintf(file, "%7.2f ", A[i * m + j]);  // %7.2f para dar formato y que se vea ordenado
        }
        fprintf(file, "\n");  // Nueva línea al final de cada fila
    }

    fclose(file);
}

// Rutina para guardar una matrix en un archivo
void saveMatrixToFile(int** A, int n, int m, const char *filename) {
    FILE *file = fopen(filename, "w");
    
    if (file == NULL) {
        printf("Error al abrir el archivo.\n");
        return;
    }

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            fprintf(file, "%d ", A[i][j]);  // %7.2f para dar formato y que se vea ordenado
        }
        fprintf(file, "\n");  // Nueva línea al final de cada fila
    }

    fclose(file);
}

// Funcion para realizar un arreglo con cierta separacion
void linspace(double x[], double init, double end, int n) {
    double delta = (end - init) / (n - 1);

    for (int i = 0; i < n; i++) {
        x[i] = init + delta * i;
    }
}

// Función para inicializar matrices
double** createMatrix(int n, int m) {
    double **Matrix = (double **)malloc(n * sizeof(double *));
    if (Matrix == NULL) {
        fprintf(stderr, "Error al asignar memoria\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        Matrix[i] = (double *)malloc(m * sizeof(double));
        if (Matrix[i] == NULL) {
            fprintf(stderr, "Error al asignar memoria en la fila %d\n", i);
            // Limpieza parcial en caso de error
            for (int j = 0; j < i; j++) {
                free(Matrix[j]);
            }
            free(Matrix);
            exit(1);
        }
    }

    return Matrix;
}

// Función para liberar la memoria de una matriz
void freeMatrix(double **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]); // Libera cada fila de la matriz
    }
    free(matrix); // Libera el puntero a los punteros de las filas
}

// Rutina para aplanar una matriz
double *FlattenMatrix(double **matrix, int rows, int cols) {
    double *flat = (double *)malloc(rows * cols * sizeof(double));
    if (flat == NULL) {
        // Manejo de error en caso de que malloc falle
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            flat[i * cols + j] = matrix[i][j];
        }
    }
    return flat;
}

// Rutina que implementa el metodo del gradiente
void Conjugate_gradient(double *A, double *B, double *x, int rows, int cols){
    double error_threshold = 0.0001;
    double alpha, beta, r_dot, r_next_dot;

    double *r = (double *)malloc(rows * sizeof(double));
    double *r_next = (double *)malloc(rows * sizeof(double));
    double *p = (double *)malloc(rows * sizeof(double));
    double *Ap = (double *)malloc(rows * sizeof(double));

    // r_0 = B - A*x_0
    double *Ax = (double *)malloc(rows * sizeof(double));
    MatrixProduct(A, x, Ax, rows, cols, 1);
    for(int i = 0; i < rows; i++) {
        r[i] = B[i] - Ax[i];
    }

    for(int i = 0; i < rows; i++){
        p[i] = r[i];  // p_0 = r_0
    }

    int max_iterations = rows; 
    for(int k = 0; k < max_iterations; k++){

        MatrixProduct(A, p, Ap, rows, cols, 1);

        r_dot = DotProd(r, r, rows);
        alpha = r_dot / DotProd(p, Ap, rows);

        for(int i = 0; i < rows; i++){
            x[i] += alpha * p[i];
            r_next[i] = r[i] - alpha * Ap[i];
        }

        r_next_dot = DotProd(r_next, r_next, rows);
        if(sqrt(r_next_dot) < error_threshold){
            break;
            printf("El método convergió con %d iteraciones\n", k);
        }

        beta = r_next_dot / r_dot;
        for(int i = 0; i < rows; i++){
            p[i] = r_next[i] + beta * p[i];
            r[i] = r_next[i];
        }
    }

    free(r);
    free(r_next);
    free(p);
    free(Ap);
    free(Ax);
}

// Rutina que indica si un vector X es solución de un sistema Ax = b
int isSolution(double *A, double *X, double *b, int size, double tolerance) {
    double *AX = malloc(size * sizeof(double));

    // Calcular AX
    MatrixProduct(A, X, AX, size, size, 1);

    // Comparar AX con b
    for (int i = 0; i < size; i++) {
        if (fabs(AX[i] - b[i]) > tolerance) {
            free(AX);
            return 1;  // Si alguna componente difiere de la tolerancia, X no es solución
        }
    }

    free(AX);
    return 0;  // Si todas las componentes son cercanas, X es solución
}

double ConditionCheck(double ta[], double td[], int n)
{

    // Definiimos el error
    double error = 1*exp(-5);
    
    // Apuntamos
    double* taPtr = ta;
    double* tdPtr = td;

    int count = 0, r = 0, i = 0;

    do {

        // El posible resultado de r es 0 o 1. Si se cumple (1) si no (0)
        r = fabs(*tdPtr ++ - *taPtr ++) <= error;
        count += r;

        i++;

    } while(i<n && r != 0 );

    // Regresamos un valor binario, 1 si son iguales 0 si no
    return count == n;


}

// Incializar una matriz cuadrada en ceros
void Matrix_Initialize(double** A, int size){

    // Inicializar la matriz a cero
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            A[i][j] = 0;
        }
    }
}

// Función para resolver un sistema Ax = b dado L en la descomposición Cholesky A = LL^T
void solveCholesky(double L[], double b[], double x[], int n) {
    // Resolvemos Ly = b para y
    double y[n];
    for (int i = 0; i < n; i++) {
        double sum = 0;
        for (int j = 0; j < i; j++) {
            sum += L[i * n + j] * y[j];
        }
        y[i] = (b[i] - sum) / L[i * n + i];
    }

    // Resolvemos L^Tx = y para x
    for (int i = n - 1; i >= 0; i--) {
        double sum = 0;
        for (int j = i + 1; j < n; j++) {
            sum += L[j * n + i] * x[j]; // Notar que estamos accediendo a L^T
        }
        x[i] = (y[i] - sum) / L[i * n + i];
    }
}

// Función para realizar la factorización de Cholesky (A = L * L^T)
int cholesky(double A[], double L[], int n) {
    // Iteramos a través de cada fila y columna para llenar L
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            double sum = 0;

            // Si estamos en la misma fila y columna (diagonal)
            if (j == i) {
                for (int k = 0; k < j; k++) {
                    sum += L[j * n + k] * L[j * n + k];
                }

                // La raíz cuadrada podría fallar si la matriz no es definida positiva
                double diagonalElement = A[i * n + i] - sum;
                if (diagonalElement <= 0) {
                    return -1; // Devolvemos -1 si hay un error (matriz no es definida positiva)
                }
                L[i * n + j] = sqrt(diagonalElement);
            } else {
                // Si estamos en diferentes filas y columnas
                for (int k = 0; k < j; k++) {
                    sum += (L[i * n + k] * L[j * n + k]);
                }
                L[i * n + j] = (A[i * n + j] - sum) / L[j * n + j];
            }
        }
    }
    return 0; // Todo bien
}

// Esta rutina calcula el metodo de potencias.
void PowerMethod(double M[], double U[], double l[], int ST)
{

    // Inicializamos las matrices
    // Asignamos memoria en el heap para nuestras matrices
    double* St = (double*) malloc(ST * sizeof(double));
    double* t = (double*) malloc(ST * sizeof(double));
    double* nt = (double*) malloc(ST * sizeof(double));
    double* v = (double*) malloc(ST * sizeof(double));
    double* vprime = (double*) malloc(ST * sizeof(double));
    double* vvp = (double*) malloc(ST * ST * sizeof(double));
    double* Snew = (double*) malloc(ST * ST * sizeof(double));

    // Apuntamos
    double* lPtr = l;
    double* UPtr = U;

    // Declaramos un nuevo vector en el que guardaremos la nueva S
    Divide(M, 1, Snew, ST*ST);


    int i = 0, j = 0, k; 

    do {

        // Calculamos St = sum(S)
        Sum(Snew,St, ST);
        // Incializamos la matriz de unos
        Ones(t, ST);

        do {
          
          // nt = St/max(St)
          Divide(St, maximus(St, ST), nt, ST);

          // Condicion de paro
          k = ConditionCheck(t, nt, ST);
          if (k == 1) {
            break;

          }

          // t = nt
          MatrixT(ST, 1, nt, t);
          // St = S*t
          MatrixProduct(Snew, t, St, ST, ST, 1);
          j ++;

        } while(j<1000);

        // Calculamos las U(:,i) = t/norm(t)
        Divide(t, sqrt(DotProd(t, t, ST)), UPtr, ST);

        // Calculamos las v(:,i) = sqrt(max(St)) * U(:,i)
        Divide(UPtr, 1/sqrt(maximus(St, ST)), v, ST);

        // Calculamos las l(i) = max(St)
        *lPtr = maximus(St, ST);

        // Redefinimos la matriz S = S - v(:,i)*v(:,i)'
        MatrixT( ST, 1, v, vprime );
        MatrixProduct( v, vprime, vvp, ST, 1, ST );
        // Copiamos la version de la matriz Snew en Scov
        // Divide(Snew, 1, M, ST*ST);
        MatrixSum(Snew, vvp, Snew, ST);

        UPtr += ST;
        lPtr ++;

        j = 0;
        i++;

    } while(i<ST);

    // Libera la memoria
    free(St);
    free(t);
    free(nt);
    free(v);
    free(vprime);
    free(vvp);
    free(Snew);

}
