
# include <stdbool.h>

// Rutina que genera una malla equidistante. Regresa un vector con la malla generada
void MallaGeneratorEquidist(int Dim, int NElements, double* malla, double x_0, double x_n);

// Funciones de forma para elementos lineales 1D
double N1(double ei);

double N2(double ei);

double dN1de(double ei);

double dN2de(double ei);

// Función que regresa la inversa del jacobiano en una dimensión
double invjacobian1d(double x1, double x2);

// Función que regresa el determinante del jacobiano en una dimensión
double detJacobian1d(double x1, double x2);

// Construcción de la matriz B para un elemento
void BuildB1d(double** B, double **DNDE, double x1, double x2, int dim, int NElements);

// Construcción del vector F para un elemento con Q constante
void BuildF1d(double* F, double** N, double Q, double x1, double x2, int NElements );

// Estructura para almacenar las condiciones de Dirichlet y Neumann
typedef struct {
    float dirichlet[2]; // Valores de las condiciones de Dirichlet
    bool dirichletValid[2]; // Indicadores para saber si las condiciones de Dirichlet son válidas
    float neumann[2]; // Valores de las condiciones de Neumann
    bool neumannValid[2]; // Indicadores para saber si las condiciones de Neumann son válidas
} BoundaryConditions;

// Función para leer las condiciones del archivo
BoundaryConditions* readConditions(const char *filename);

// Rutina que lee la malla y da los parámetros del problema
void ProblemDefinition(const char *filename, int* dim, int* NNodes, int* NElements, int* NNodes_Elemento);

// Rutina que lee la malla y da los parámetros del problema
void ReadMesh(const char *filename, double *nodos, int** elementos, int dim, int NNodes, int NElements, int NNodes_Elemento);