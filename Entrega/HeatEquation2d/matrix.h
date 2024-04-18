

// Esta rutina muestra una matriz de mxn
void MatrixShow(int m, int n, double **v);

// Esta rutina muestra un vector de mxn
void VectorShow(int m, int n, double *v);

// Esta rutina calcula la matriz transpuesta de una matriz y la guarda en otro
void MatrixT(int rows, int cols, double **matrix, double **transposed);

// Esta rutina calcula la matriz transpuesta de un vector y la guarda en otro
void VectorT(int n, int m, double v[], double Vec[] );

// Esta rutina inicializa a cero un arreglo
void Initialize(double in[], int n);

// Esta rutina calcula la suma de dos matrices cuadradas
void MatrixSum(double m1[], double m2[], double mout[], int n);

// Esta rutina calcula el producto punto entre dos vectores
double DotProd(double x[], double y[], int n);

// Esta rutina hace el producto entre dos matrices
void MatrixProduct(double **m1, double **m2, double **result, int M, int N, int O);

// Esta rutina hace el producto entre dos matrices
void VectorProduct(double m1[], double m2[], double Matrix[], int M, int N, int O);

// Rutina para leer una matriz
int ReadMatrix(const char *filename, int **A, int rows, int cols);

// Rutina para leer un vector desde un archivo
int ReadVector(const char *filename, double *A, int n);

// Este programa calcula el valor maxino de un arreglo
double maximus(double v[], int s);

// Este programa suma todos los elementos de un vector, y los pone en otro vec
void Sum(double in[], double out[], int n);

// Esta rutina hace la multiplicación de una matriz cuadrada por un escalar
void MatriXEscalar(double** mat, double x, int rows, int cols);

// Este programa divide un arreglo por un escalar
void Divide(double matrix[], double scalar, double out[], int s);

// Este programa multiplica un arreglo por un escalar
void VectorXEscalar(double matrix[], double scalar, double out[], int s);

// Esta rutina inicializa un arreglo de unos
void Ones(double t[], int n);

// Rutina para restar v2 de v1 (v1 = v1 - v2)
void SubtractVector(double *v1, double *v2, int n);

// Rutina para guardar un vector en un archivo
void saveVectorToFile(double *A, int n, int m, const char *filename);

// Rutina para guardar una matrix en un archivo
void saveMatrixToFile(int** A, int n, int m, const char *filename);

// Funcion para realizar un arreglo con cierta separacion
void linspace(double x[], double init, double end, int n);

// Función para inicializar matrices
double** createMatrix(int n, int m);

// Función para liberar la memoria de una matriz
void freeMatrix(double **matrix, int rows);

// Rutina para aplanar una matriz
void FlattenMatrix(double **matrix, int rows, int cols, double *flat);

// Rutina que implementa el metodo del gradiente
void Conjugate_gradient(double *A, double *B, double *x, int rows, int cols);

// Rutina que indica si un vector X es solución de un sistema Ax = b
int isSolution(double *A, double *X, double *b, int size, double tolerance);

// Incializar una matriz en ceros
void Matrix_Initialize(double** A, int rows, int cols);

// Función para resolver un sistema Ax = b dado L en la descomposición Cholesky A = LL^T
void solveCholesky(double L[], double b[], double x[], int n);

// Función para realizar la factorización de Cholesky (A = L * L^T)
int cholesky(double A[], double L[], int n);
