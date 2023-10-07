/* File:     riemann_mpi.c
 *
 * Compile:  mpicc -g -Wall -o riemann_mpi riemann_mpi.c -lm
 * Run:      mpiexec -n <comm_sz> ./riemann_mpi
 *
 * STEFANO ALBERTO ARAGONI MALDONADO
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

/*-------------------------------------------------------------------
Función:    Read_n
Propósito:  Lee input del usuario
 */
void Read_n(
      int*      a,            /* out */
      int*      b,            /* out */
      int*      funcion,      /* out */

      int       my_rank    /* in  */,
      int       comm_sz    /* in  */,
      MPI_Comm  comm       /* in  */) {


      if (my_rank == 0) {
            printf("Cual es el limite inferior de la integral?: ");
            fflush(stdout);
            scanf("%d", a);

            printf("Cual es el limite superior de la integral?: ");
            fflush(stdout);
            scanf("%d", b);

            printf("Funciones:\n1. x^2\n2. 2x^3\n3. sin(x)\n Cual funcion desea integrar?:");
            fflush(stdout);
            scanf("%d", funcion);
      }

      MPI_Bcast(a, 1, MPI_INT, 0, comm);
      MPI_Bcast(b, 1, MPI_INT, 0, comm);
      MPI_Bcast(funcion, 1, MPI_INT, 0, comm);

} 

/*-------------------------------------------------------------------
Función:    f
Propósito:  Regresa el valor de la función dependiendo de la opción
Entradas:   x: valor de x
            option: función a integrar
*/
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


/*-------------------------------------------------------------------
Función:    trapezoides
Propósito:  Calcula la integral de la función a partir de la regla de los trapecios
Entradas:   a: limite inferior de la integral
            b: limite superior de la integral
            n: numero de trapecios
            option: función a integrar
*/
double trapezoides(
            int         a,          /* in  */
            int         b,          /* in  */
            int         n,          /* in  */
            int         option,     /* in  */

            int         my_rank,    /* in  */
            int         comm_sz,    /* in  */
            MPI_Comm    comm        /* in  */) {


      long double h = (b - a) / (long double) n;         // Calculo de H
      
      double local_n = 0;
      double a_local = 0;
      double b_local = 0;

      local_n = n / comm_sz;      // Calcula el numero de trapezoides que le toca a cada thread
      a_local = a + (my_rank * local_n * h);    // Calcula el limite inferior de cada thread
      b_local = a_local + (local_n * h);          // Calcula el limite superior de cada thread

      double integral = (f(a_local, option) + f(b_local, option))/ 2.0; // Se suman los limites f(a) y f(b) y se dividen entre 2.

      for (int i = 1; i < local_n; i++) {     // Se calcula la suma de los f(x) de cada thread
            double x = a_local + i * h;
            integral += f(x, option);
      }

      integral *= h;  // Se multiplica por h

      return integral;
}


/*-------------------------------------------------------------------*/
int main(void) {

      int n = 1000000;    // 10e6 
      int a = 0;           
      int b = 10;      // Intervalo default de 0 a 10
      int funcion = 1;        // Default de la función x^2
      
      int comm_sz, my_rank;                        // comm_sz = número de procesos, my_rank = rango del proceso
      MPI_Comm comm;     

      MPI_Init(NULL, NULL);                        // Inicializa el ambiente de MPI
      comm = MPI_COMM_WORLD;
      MPI_Comm_size(comm, &comm_sz);               // Obtiene el número de procesos
      MPI_Comm_rank(comm, &my_rank);               // Obtiene el rango del proceso


      Read_n(&a, &b, &funcion, my_rank, comm_sz, comm);  // Se lee el input del usuario

      if (n % comm_sz != 0) {   // Revisa que n sea divisible entre el numero de threads
            printf("El numero de trapezoides debe ser divisible entre el numero de threads\n");
            return 0;
      }

      double start = clock();

      double integral_global = 0.0;  // Variable global para guardar la integral
      double integral = trapezoides(a, b, n, funcion, my_rank, comm_sz, comm);     // Manda a calcular la integral de la funcion
      MPI_Reduce(&integral, &integral_global, 1, MPI_DOUBLE, MPI_SUM, 0, comm);   // Reduce la integral de cada proceso en la variable global

      double finish = clock();
      double elapsed = (double)(finish - start) / CLOCKS_PER_SEC;

      if (my_rank == 0) {
            printf("\nTiempo: %f\n", elapsed);
            
            printf("Con n = %d trapezoides, nuestra aproximación \n", n);
            printf("de la integral de %d a %d es = %f\n", a, b, integral_global);
      }

      MPI_Finalize();

      return 0;
}  /* main */
