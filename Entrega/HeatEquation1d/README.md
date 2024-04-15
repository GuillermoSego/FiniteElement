# Ecuación de calor en una dimensión - Elemento Finito I

En este readme se indica como es la compilación y ejecución, además de la estructura de los archivos que conforman el HeatEquation2d.

## Requisitos

- GCC o cualquier otro compilador compatible.
- Herramientas estándar de Unix/Linux para compilar y ejecutar.

## Compilación

Para compilar los programas, se ha proporcionado un archivo `Makefile`. El makefile, automáticamente crea las carpetas `build/` necesaria para almacenar los ejecutables de los programas. Para correrse, simplemente ejecute el siguiente comando en la terminal.

```
make
```

Esto generará los archivos binarios necesarios en la carpeta `build/Debug/`.

## Ejecución

Para ejecutar el programa se tiene que agregar por línea de comandos adicionalmente un archivo.dat con toda la información de malla, conectividades, materiales y condiciones de contorno, todo establecido en el .bas del proyecto. Una vez compilado el programa, se puede ejecutar de la siguiente manera:

```
./build/Debug/MainHeatEquation1d Line.dat
```