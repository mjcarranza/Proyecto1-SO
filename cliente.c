/*
    proceso encargado de leer un archivo.txt y escribir cada
    uno de los caracterees en la memoria compartida
    inicializada porel proceso creador
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>     // Incluir para la función sleep
#include <semaphore.h>  // Para utilizar semaforos
#include <fcntl.h>      // para la creacion del semaforo

#define MEM_SIZE 20  // Tamaño de la memoria compartida en bytes
#define MAX_SIZE 1000  // Tamaño máximo del archivo

sem_t *semaforo;        // instancia de semaforo

int main() {
    FILE *archivo;
    char caracter;
    char buffer[MAX_SIZE];
    
    /*----------------Pedir al usuario el nombre del archivo----------------*/
    char direccion[50] = "archivos/";
    char nombre[50]; // Variable para almacenar el nombre del archivo
    printf("Ingrese el nombre del archivo a abrir (no especifique la extension del archivo): ");
    scanf("%s", nombre); // Lee el nombre ingresado por el usuario y lo guarda en la variable 'nombre'
    strcat(nombre, ".txt");
    strcat(direccion, nombre); // la variable direccion tiene la direccion del archivo que se va a abrir

    /*---------------------Abre el archivo en modo lectura--------------------*/
    archivo = fopen(direccion, "r");

    // Verifica si se pudo abrir el archivo correctamente
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return 1;
    }

    /*--------------------ABRIR EL SEMAFORO CON EL NOMBRE----------------------*/
    semaforo = sem_open("/mysemaphore", O_CREAT, 0644, 1); // "/mysemaphore" es el nombre del semaforo. Utilizarlo para acceder a el en los demas procesos
    if (semaforo == SEM_FAILED) { // en caso de que falle la creacion del semaforo
        perror("sem_open");
        exit(EXIT_FAILURE);
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
    
    /* --------------------------- leer contenido del archivo ----------------------------*/
    // Leer el contenido del archivo en el buffer
    size_t bytesLeidos = fread(buffer, sizeof(char), MAX_SIZE, archivo);
    int *ptr_entero = (int *)memoria;
    if (bytesLeidos == 0) {
        perror("Error al leer el archivo");
        return 1;
    }

    /*---------------------------escribir en la memoria compartida------------------------*/
    // hacer esto llamando una funcion para que quede mejor lo del semaforo

    for (int i = 0; i < bytesLeidos; i++)
    {
        for (int j = 0; j < MEM_SIZE; j++)
        {
            // Acceder al carácter en el índice especificado
            caracter = buffer[i];
            printf("%c",caracter);
            sleep(3);
            ptr_entero[j] = caracter;
        }
    }
    



    /*----------------------Desadjuntar la memoria compartida--------------------------*/
    if (shmdt(memoria) == -1) {
        perror("Error al desadjuntar la memoria compartida");
        exit(1);
    }

    // Cierra el archivo 
    fclose(archivo);

    return 0;
}


/*
  // Escribir en la memoria compartida
  int *ptr_entero = (int *)ptr_memoria;
  for (int i = 0; i < num_caracteres; i++) {
    ptr_entero[i] = 0;
  }
*/
