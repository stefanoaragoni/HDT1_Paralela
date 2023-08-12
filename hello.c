#include <stdio.h>
#include <stdlib.h>
#include "omp.h"

void hello_world(void);
    
int main(int argc, char* argv[]) {
    int thread_count = 10;
    
    if(argc > 1){
        thread_count = strtol(argv[1], NULL, 10);
    }

    #pragma omp parallel num_threads(thread_count)
    hello_world();
    return 0;
}

void hello_world(void) {
    printf("Hello World!\n");
}