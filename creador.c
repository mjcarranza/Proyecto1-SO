#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>  // Incluir para la función sleep


// Definir tamaño de entero
#define TAM_ENTERO 4
// Definir tamaño de caracter
#define TAM_CHAR 1

int main() {
  // Pedir al usuario la cantidad de caracteres
  int num_caracteres;
  printf("Ingrese la cantidad de caracteres a compartir: ");
  scanf("%d", &num_caracteres);

  // Calcular el tamaño total de la memoria (4 bytes por caracter)
  int tamanio_memoria = num_caracteres * TAM_ENTERO;

  // Crear la memoria compartida
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
  printf("La dirección de memoria es: %p\n", ptr_memoria);
  if (ptr_memoria == (void *)-1) {
    perror("Error al mapear la memoria compartida");
    exit(1);
  }

  // Inicializar la memoria compartida
  int *ptr_entero = (int *)ptr_memoria;
  for (int i = 0; i < num_caracteres; i++) {
    ptr_entero[i] = 0;
  }

  // Visualizar la memoria compartida en tiempo real
  while (1) {
    // Mostrar el contenido de la memoria
    for (int i = 0; i < num_caracteres; i++) {
      printf("%d ", ptr_entero[i]);
    }
    printf("\n");

    // Simular tiempo real (esperar 1 segundo)
    sleep(1);
  }

  // 7. Desvincular la memoria compartida del espacio de direcciones del proceso
  shmdt(ptr_memoria);

  // 8. Eliminar la memoria compartida
  shmctl(id_memoria, IPC_RMID, NULL);

  return 0;
}


// esta es la direccion de memoria donde se inicia la memoria compartida --> 0x7ffff7ffa000