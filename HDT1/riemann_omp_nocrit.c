// Stefano Alberto Aragoni Maldonado (20261)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

double f(double x, int option);
void trapezoides(double a, double b, int n, int option, double* integral_global);

int main(int argc, char* argv[]) {
    int n = 1000000;    // 10e6
    double a = 0;
    double b = 10;      // Intervalo default de 0 a 10
    int thread_count = 10;   // Default de 10 threads

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

    double integral[thread_count];      // Arreglo de integrales de cada thread

    #pragma omp parallel num_threads(thread_count)      // Distribuye el trabajo en los threads
    trapezoides(a, b, n, opcion, integral);            // Manda a calcular la integral de la funcion


    double integral_final;

    for (int i = 0; i < thread_count; i++) {     // Suma las integrales de cada thread
        integral_final += integral[i];
    }


    printf("Con n = %d trapezoides, nuestra aproximación \n", n);
    printf("de la integral de %f a %f es = %f\n", a, b, integral_final);

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

    integral_global[thread_ID] = integral;   // Se guarda la integral en el arreglo global
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
