// Stefano Alberto Aragoni Maldonado (20261)

#include <stdio.h>
#include <stdlib.h>
#include "omp.h"

void hello_world(int thread_num, int thread_count);
    
int main(int argc, char* argv[]) {
    int thread_count = 10;
    
    // Revisa que se haya ingresado un valor.
    if (argc > 1) {
        thread_count = strtol(argv[1], NULL, 10);
    }

    #pragma omp parallel num_threads(thread_count)
    hello_world(omp_get_thread_num(), thread_count);    // Imprime la cantidad de hilos

    return 0;
}

void hello_world(int thread_num, int thread_count) {
    // Recibe la el numero de hilo actual, asi como la cantidad de hilos

    if (thread_num % 2 == 0) {   // Si el id es par, imrpime saludos
        printf("Saludos del hilo %d\n", thread_num);
        
    } else {                    // Si el id es impar, imprime feliz cumplea;os
        printf("Feliz cumpleaños número %d!\n", thread_count);
    }
}