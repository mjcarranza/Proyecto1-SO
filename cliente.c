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

#define MEM_SIZE 20     // Tamaño de la memoria compartida en bytes
#define MAX_SIZE 1000   // Tamaño máximo del archivo

char buffer[MAX_SIZE];
char caracter;          // variable para guardar el caracter leido
sem_t *semaforo;        // instancia de semaforo
int contador = 0;       // contador para controlar los caracteres leidos
size_t bytesLeidos;     // longitud del buffer
int *ptr_entero;        // puntero para la memoria compartida


int verificaEscritura(){
    // si no han terminado de trqnsferir los caracteres, continuar con escritura en memoria
    if (contador < bytesLeidos)
    {
        regCrit();
    }
    //
    else{
        printf("Se acabo el documento \n");
        return 0;
    }
    
}

/*FUNCION PARA ENTRAR A LA REGION CRITICA*/
void regCrit(){
    
    if (ptr_entero[0] == 0)                 // escribir caracter en campo de memoria
    {
        sem_wait(&semaforo);                // pedir semaforo
        for (int i = 1; i < MEM_SIZE; i++){ // iniciar escritura en memoria
            caracter = buffer[contador];    // Acceder al carácter en el índice especificado

            printf("%c",caracter);          // imprimir lo necesario
            //sleep(3);

            ptr_entero[i] = caracter;       // puntero a la direccion de memoria compartida
            contador ++;                    // aumento el contador del buffer
        }
        ptr_entero[0] = 1;                  // dar senal para leer
        sem_post(&semaforo);                // liberar el semaforo
    }
    //verificaEscritura();                    // verificar si ya se termino el proceso 
}


int main() {

    FILE *archivo;
    
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
    semaforo = sem_open("/mysemaphore", 0); // "/mysemaphore" es el nombre del semaforo. Utilizarlo para acceder a el en los demas procesos
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
    bytesLeidos = fread(buffer, sizeof(char), MAX_SIZE, archivo);
    ptr_entero = (int *)memoria;    // ver si debo poner el asterisco antes de ptr
    if (bytesLeidos == 0) {
        perror("Error al leer el archivo");
        return 1;
    }

    verificaEscritura();            // empezar a escribir los caracteres en memoria


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
