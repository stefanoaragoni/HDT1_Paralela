/* File:     riemann.c
 *
 * Compile:  gcc -o riemann riemann.c -lm
 * Run:      ./riemann a b
 *
 * STEFANO ALBERTO ARAGONI MALDONADO
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double f(double x, int option);
double trapezoides(double a, double b, int n, int option);

int main(int argc, char* argv[]) {
    long int n = 10000000000;  //1e10
    double a = 0;
    double b = 10; // Intervalo default de 0 a 10

    // Scanf para preguntar qué función se quiere integrar
    printf("Funciones:\n1. x^2\n2. 2x^3\n3. sin(x)\n");
    int opcion;
    scanf("%d", &opcion);

    // Revisa que se haya ingresado valores
    if (argc > 2) {
        a = strtod(argv[1], NULL);
        b = strtod(argv[2], NULL);
    }

    double start = clock();

    double integral = trapezoides(a, b, n, opcion);     // Manda a calcular la integral de la funcion

    double finish = clock();
    double elapsed = (double)(finish - start) / CLOCKS_PER_SEC;
    printf("Tiempo: %f\n", elapsed);

    printf("Con n = %ld trapezoides, nuestra aproximación \n", n);
    printf("de la integral de %f a %f es = %f\n", a, b, integral);

    return 0;
}

double trapezoides(double a, double b, int n, int option) {
    double h = (b - a) / n;         // Calculo de H
    double integral = 0;

    integral += (f(a, option) + f(b, option))/ 2.0; // Se suman los limites f(a) y f(b) y se dividen entre 2

    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        integral += f(x, option);
    }

    integral *= h;      // Se multiplica por h

    return integral;
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
