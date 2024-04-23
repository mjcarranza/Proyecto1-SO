/*
    proceso encargado de leer un archivo.txt y escribir cada
    uno de los caracterees en la memoria compartida
    inicializada porel proceso creador
*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include <unistd.h>     // Incluir para la función sleep
#include <semaphore.h>  // Para utilizar semaforos
#include <fcntl.h>      // para la creacion del semaforo

#define MEM_SIZE 35     // Tamaño de la memoria compartida en bytes
#define MAX_SIZE 1000   // Tamaño máximo del archivo

char buffer[MAX_SIZE];
char caracter;          // variable para guardar el caracter leido
sem_t semaforo;         // instancia de semaforo
size_t bytesLeidos;     // longitud del buffer
int contador;           // contador para conrolar los espacios de memoria donde se escribe
int *ptr_entero;        // puntero para la memoria compartida
char caracterFile;

int main() {

    FILE *archivo;
    clock_t tu, tu_end, tk, tk_end, tBlock, tBlock_end; 
    
    /*----------------Pedir al usuario el nombre del archivo----------------*/
    char direccion[50] = "archivos/";
    char nombre[50]; // Variable para almacenar el nombre del archivo
    char modo[50];
    printf("Ingrese el nombre del archivo a abrir (no especifique la extension del archivo): ");
    scanf("%s", nombre); // Lee el nombre ingresado por el usuario y lo guarda en la variable 'nombre'
    strcat(nombre, ".txt");
    strcat(direccion, nombre); // la variable direccion tiene la direccion del archivo que se va a abrir

    printf("Ingrese el modo en que quiere ejecutar el proceso (automatico o manual): ");
    scanf("%s", modo);

    /*---------------------Abre el archivo en modo lectura--------------------*/
    archivo = fopen(direccion, "r");

    // Verifica si se pudo abrir el archivo correctamente
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return 1;
    }


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
    
    /* --------------------------- leer contenido del archivo ----------------------------*/
    // Leer el contenido del archivo en el buffer
    bytesLeidos = fread(buffer, sizeof(char), MAX_SIZE, archivo);
    ptr_entero = (int *)memoria;    // ver si debo poner el asterisco antes de ptr
    if (bytesLeidos == 0) {
        perror("Error al leer el archivo");
        return 1;
    }

    printf("\n\n");
    printf("El texto completo es: \n");
    for (int i = 0; i < bytesLeidos; i++)
    {
        printf("%c", buffer[i]);
    }
    printf("\n\n");

    ptr_entero[15] = 16; // inicializar el contador de posicion en memoria en cero
    printf("contadooor: %d\n",ptr_entero[15]);

    while (1)
    {
        tBlock = clock();
        sem_wait(&semaforo);                                                    // pedir semaforo
        tBlock_end = clock();
        ptr_entero[2] = ptr_entero[2] + ((double) (tBlock_end-tBlock)) / CLOCKS_PER_SEC;

        if (ptr_entero[0] < bytesLeidos){                                       // verificar si ya se termino de leer el archivo
            contador = ptr_entero[15];
            if (contador <= MEM_SIZE)                                            // si se cumple, se escribe (memoria compartida para caracteres empieza en posicion 1 ==> contador = 1)
            {
                tk = clock();
                caracter = buffer[ptr_entero[0]];                               // Acceder al carácter en el índice especificado
                ptr_entero[contador] = caracter;                                // escribir en al direccion de memoria compartida
                tk_end = clock();
                ptr_entero[7] = ptr_entero[7] + ((double) (tk_end-tk)) / CLOCKS_PER_SEC; // Segundos;

                // obtener tamano del caracter
                tk = clock();
                ptr_entero[5] = ptr_entero[5] + sizeof(caracter);
                tk_end = clock();
                ptr_entero[7] = ptr_entero[7] + ((double) (tk_end-tk)) / CLOCKS_PER_SEC; // Segundos;

                printf("El caracter introducido es: %c\n",caracter);            // imprimir lo necesario

                
                
                time_t now = time(NULL);                                        // Obtener el tiempo actual
                struct tm *local = localtime(&now);                             // Convertir time_t a estructura tm como hora local
                printf("Hora de insersión en memoria: %02d:%02d:%02d\n", local->tm_hour,  local->tm_min, local->tm_sec);  //horas:minutos:segundos
                
                int memPos = (int *)memoria+contador;
                printf("Posición en memoria donde se introdujo el caracter: 0x%X\n\n", memPos);

                tk = clock();
                ptr_entero[0] = ptr_entero[0] + 1;  // aumentar la posicion en la que se debe leer del buffer
                tk_end = clock();
                ptr_entero[7] = ptr_entero[7] + ((double) (tk_end-tk)) / CLOCKS_PER_SEC; // Segundos;

                ptr_entero[15] = ptr_entero[15] + 1;                        // aumentar el contador
                printf("El contador es: %d\n", ptr_entero[0]);
                printf("El contador de for es: %d\n", ptr_entero[15]);
                sleep(2);
                
            }
            else{                                   // volver al inicio de la memoria compartida para los caracteres
                //sleep(1);
                ptr_entero[15] = 16;
            }

            
        }
        else{
            ptr_entero[1] = 1;
            printf("Se acabo el documento \n");
            sem_post(&semaforo);                // liberar el semaforo
            break;
        }
        sem_post(&semaforo);                // liberar el semaforo
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
  preguntar como puedo hacer lo de bloquear el proceso
  porque a como esta no funciona simplemente hacerlo por medio de funciones

  ver apuntes en tablet
*/
