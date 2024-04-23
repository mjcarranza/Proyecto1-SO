/*
    proceso encargado de leer de memoria compartida y escribir cada
    uno de los caracterees en un nuevo archivo igual al original
*/

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


char caracter;          // variable para guardar el caracter leido
sem_t semaforo;         // instancia de semaforo
int *ptr_entero;        // puntero para la memoria compartida
#define MEM_SIZE 22     // Tamaño de la memoria compartida en bytes

int main() {
    
    /*----------------Nombre del archivo reconstruido----------------*/
    char direccion[50] = "archivos/";
    char nombre[50] = "texto_reconstruido"; // Variable para almacenar el nombre del archivo
    strcat(nombre, ".txt");
    strcat(direccion, nombre); // la variable direccion tiene la direccion del archivo que se va a escribir

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
    
    /* --------------------------- Leer contenido de memoria ----------------------------*/
    ptr_entero = (int *)memoria;    // ver si debo poner el asterisco antes de ptr

    while (1)
    {   
        printf("Enter para tomar otro caracter!\n");
        getchar();
        sem_wait(&semaforo);  
                                                          // pedir semaforo
        int caracteres_memoria = (int) ptr_entero[13];               // Acceder al carácter en el índice especificado

        if (caracteres_memoria > 0){
            int cont_reconstructor = (int) ptr_entero[14];
            if (cont_reconstructor < MEM_SIZE){
                caracter = (char) ptr_entero[15 + cont_reconstructor];
                ptr_entero[14 + cont_reconstructor] = 0;
                ptr_entero[13] = caracteres_memoria - 1;
                ptr_entero[14] = cont_reconstructor + 1;
            }
            else{
                caracter = (char) ptr_entero[15];
                ptr_entero[15] = 0;
                ptr_entero[13] = caracteres_memoria - 1;
                ptr_entero[14] = 0;    
            }
        }else{
            printf("No hay caracteres en memoria");
        }
        sem_post(&semaforo);
        sleep(1);
        if (ptr_entero[1] == 1)
        {
        printf("\nTransferencia terminada.\n");
        break;
        } 
    }

    sem_post(&semaforo);                // liberar el semaforo
    /*----------------------Desadjuntar la memoria compartida--------------------------*/
    if (shmdt(memoria) == -1) {
        perror("Error al desadjuntar la memoria compartida");
        exit(1);
    }

    return 0;
}