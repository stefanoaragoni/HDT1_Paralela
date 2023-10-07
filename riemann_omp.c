/* File:     riemann_omp.c
 *
 * Compile:  gcc -o riemann_omp riemann_omp.c -lm -fopenmp
 * Run:      ./riemann_omp a b thread_count
 *
 * Compile on MACOS: /usr/local/opt/llvm/bin/clang -fopenmp -o riemann_omp riemann_omp.c
 * Run:      ./riemann_omp a b thread_count
 * 
 * 
 * STEFANO ALBERTO ARAGONI MALDONADO
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

double f(double x, int option);
void trapezoides(double a, double b, int n, int option, double* integral_global);

int main(int argc, char* argv[]) {
    long int n = 10000000000;  //1e10
    double a = 0;
    double b = 10;      // Intervalo default de 0 a 10
    int thread_count = 4;   // Default de 4 threads

    // Scanf para preguntar qué función se quiere integrar
    printf("Funciones:\n1. x^2\n2. 2x^3\n3. sin(x)\n");
    int opcion;
    scanf("%d", &opcion);

    // Revisa que se haya ingresado valores
    if (argc > 3) {
        a = strtod(argv[1], NULL);
        b = strtod(argv[2], NULL);
        thread_count = strtod(argv[3], NULL);
    }

    if (n % thread_count != 0) {   // Revisa que n sea divisible entre el numero de threads
        printf("El numero de trapezoides debe ser divisible entre el numero de threads\n");
        return 0;
    }   

    double integral_global = 0.0;  // Variable global para guardar la integral

    double start = omp_get_wtime();

    #pragma omp parallel num_threads(thread_count)      // Distribuye el trabajo en los threads
    trapezoides(a, b, n, opcion, &integral_global);     // Manda a calcular la integral de la funcion

    double finish = omp_get_wtime();
    double elapsed = finish - start;
    printf("\nTiempo: %f\n", elapsed);
    
    printf("Con n = %ld trapezoides, nuestra aproximación \n", n);
    printf("de la integral de %f a %f es = %f\n", a, b, integral_global);

    return 0;
}

void trapezoides(double a, double b, int n, int option, double* integral_global) {    // Recibe la dirección de la variable global
    int threads = omp_get_num_threads();    // Obtiene el numero total de threads
    int thread_ID = omp_get_thread_num();     // Obtiene el numero de thread actual

    double h = (b - a) / n;         // Calculo de H
    
    int n_local = n / threads;      // Calcula el numero de trapezoides que le toca a cada thread
    double a_local = a + (thread_ID * n_local * h);    // Calcula el limite inferior de cada thread
    double b_local = a_local + (n_local * h);          // Calcula el limite superior de cada thread

    double integral = (f(a_local, option) + f(b_local, option))/ 2.0; // Se suman los limites f(a) y f(b) y se dividen entre 2

    for (int i = 1; i < n_local; i++) {     // Se calcula la suma de los f(x) de cada thread
        double x = a_local + i * h;
        integral += f(x, option);
    }

    integral *= h;  // Se multiplica por h

    #pragma omp critical
    *integral_global += integral;   // Se suma la integral de cada thread a la variable global
}

double f(double x, int option) {

    // Dependiendo de la opción, se regresa la función correspondiente
    switch (option) {
        case 1:
            return x * x;
        case 2:
            return 2 * x * x * x;
        case 3:
            return sin(x);
        default:
            return 0;
    }
}
