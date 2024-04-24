#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include <unistd.h>     // Incluir para la función sleep
#include <semaphore.h>  // Para utilizar semaforos
#include <fcntl.h>      // para la creacion del semaforo

#define MEM_SIZE 15     // Tamaño de la memoria compartida en bytes

sem_t semaforo;         // instancia de semaforo
double *ptr_entero;        // puntero para la memoria compartida

int main() {

    /*--------------------ABRIR EL SEMAFORO CON EL NOMBRE----------------------*/
    // Inicializa el semáforo para uso entre procesos
    if (sem_init(&semaforo, 1, 1) == -1) {
        perror("sem_init"); // Imprime el error si no se pudo inicializar
        return 1;
    }

    /* ---------------- ACCEDER A MEMORIA COMPARTIDA ---------------------- */
    
    // Abre la memoria compartida
    key_t clave = 1234; // Clave para la memoria compartida
    // Acceder al segmento de memoria compartida
    int id_memoria = shmget(clave, MEM_SIZE, 0666);
    if (id_memoria == -1) {
        perror("Error al acceder a la memoria compartida");
        exit(1);
    }

    // Adjuntar el segmento de memoria compartida al espacio de direcciones del proceso
    void *memoria = shmat(id_memoria, NULL, 0);
    if (memoria == (void *) -1) {
        perror("Error al adjuntar la memoria compartida");
        exit(1);
    }

    ptr_entero = (double *)memoria;    // ver si debo poner el asterisco antes de ptr
    printf("Esperando que los procesos terminen.... \n\n");

    while (1)
    {
        /* revisar que hayan terminado todos los procesos, si terminaron*/
        if ((ptr_entero[1] == 1 && ptr_entero[10] == 1) )  // && ptr_entero[10] == 1 si ya terminaron el cliente y el reconstructor
        {
            sem_wait(&semaforo); // pedir semaforo
            // mostrar las estadisticas segun los valores encontrados en memoria
            printf("--------> Estadísticas de los procesos --------> \n\n");
            printf("Tiempo bloqueado del Cliente: %0.10f segundos.\n", ptr_entero[2]);              //
            printf("Tiempo bloquedado del Reconstructor: %0.10f segundos.\n", ptr_entero[3]);       // server
            printf("Número de caracteres transferidos: %f caracteres.\n", ptr_entero[0]);           //
            printf("Número de caracteres en el buffer: %f caracteres.\n", ptr_entero[13]);           // server
            printf("Espacio total de memoria utilizado: %f bytes.\n", ptr_entero[5]);               //
            printf("Tiempo del Cliente en modo Usuario: %0.10f segundos.\n", ptr_entero[6]);
            printf("Tiempo del Cliente en modo Kernel: %0.10f segundos.\n", ptr_entero[7]);         //
            printf("Tiempo del Reconstructor en modo Usuario: %0.10f segundos.\n", ptr_entero[8]);
            printf("Tiempo del Reconstructor en modo Kernel: %0.10f segundos.\n", ptr_entero[9]);
            sem_post(&semaforo); // liberar semaforo
            break;
        }
        
    }
    
    
    // ptr_entero[contador] = caracter;                                // escribir en al direccion de memoria compartida



    /*----------------------Desadjuntar la memoria compartida--------------------------*/
    if (shmdt(memoria) == -1) {
        perror("Error al desadjuntar la memoria compartida");
        exit(1);
    }

    return 0;
}