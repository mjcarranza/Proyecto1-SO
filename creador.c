#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>      // para la creacion del semaforo
#include <unistd.h>     // Incluir para la función sleep
#include <semaphore.h>  // Para utilizar semaforos


#define TAM_ENTERO 4    // Definir tamaño de entero
#define TAM_CHAR 1      // Definir tamaño de caracter

sem_t *semaforo;        // instancia de semaforo

int main() {

  /*--------------------------------Crea el semáforo con nombre--------------------------------*/
  semaforo = sem_open("/mysemaphore", O_CREAT, 0644, 1); // "/mysemaphore" es el nombre del semaforo.
  if (semaforo == SEM_FAILED) { // en caso de que falle la creacion del semaforo
      perror("sem_open");
      exit(EXIT_FAILURE);
  }

  /*---------------------------Pedir al usuario la cantidad de caracteres----------------------------*/
  int num_caracteres = 0;
  printf("Ingrese la cantidad de caracteres a compartir: ");
  scanf("%d", &num_caracteres);

  // Calcular el tamaño total de la memoria (4 bytes por caracter)
  int tamanio_memoria = (num_caracteres+15) * TAM_ENTERO;

  /*-------------------------------------Crear la memoria compartida-------------------------------------*/
  // Clave para identificar la memoria compartida
  key_t clave = 1234;
  // Crear segmento de memoria compartida
  int id_memoria = shmget(clave, tamanio_memoria, IPC_CREAT | 0666);
  if (id_memoria == -1) {
    perror("Error al crear la memoria compartida");
    exit(1);
  }

  // Mapear la memoria compartida en el espacio de direcciones del proceso
  void *ptr_memoria = shmat(id_memoria, NULL, 0);
  if (ptr_memoria == (void *)-1) {
    perror("Error al mapear la memoria compartida");
    exit(1);
  }

  // Inicializar la memoria compartida (todos los campos en 0)
  int *ptr_entero = (int *)ptr_memoria;
  for (int i = 0; i < tamanio_memoria+1; i++) {
    ptr_entero[i] = 0;
  }
  ptr_entero[12] = num_caracteres+15;
  ptr_entero[13] = 0;
  ptr_entero[15] = 16;
  printf("Imprimiendo memoria....\n");
  /*---------------------------Visualizar la memoria compartida en tiempo real---------------------------*/
  while (1) {      
    // Mostrar el contenido de la memoria
    for (int i = 16; i < tamanio_memoria+1; i++) {
      char caracter = (char) ptr_entero[i];
      printf("%c", caracter);
    }
    printf("\n");

    // si se termino la escritura, terminar el programa
    if (ptr_entero[1] == 1 )    // && ptr_entero[10] == 1 si terminaron los clientes y terminaron los reconstructores
    {
      printf("\nTransferencia terminada.\n");
      break;
    }

    // Simular tiempo real (esperar 1 segundo)
    sleep(0.5);
  }

  /*-----------------------------TERMINAR EL PROGRAMA-----------------------------*/

  shmdt(ptr_memoria);                 // Desvincular la memoria compartida del espacio de direcciones del proceso
  shmctl(id_memoria, IPC_RMID, NULL); // Eliminar la memoria compartida

  return 0;
}



/* NOTAS */
 
// esta es la direccion de memoria donde se inicia la memoria compartida 
// --> 0x7ffff7ffa000

// hacer algo para poder cerrar la ejecucion del proceso y con ello cerrar 
// el semaforo

/* hay que cambiar lo de SLEEP porque es bussy waiting */ 