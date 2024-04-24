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
#define MEM_SIZE 22     // Tamaño de la memoria compartida en bytes
char direccion[50] = "archivos/texto_reconstruido.txt";

void writeToFile(char caracter){
    FILE *archivo = fopen(direccion, "a");
                
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }
                
    // Escribir un carácter adicional al final del archivo
    fputc(caracter, archivo);

    // Cerrar el archivo
    fclose(archivo);

}

int main() {
    clock_t tu, tu_end, tk, tk_end, tBlock, tBlock_end; 
    char modo[1];

    printf("Especifique el modo de ejecución --> \n1. Automático \n2. Manual\nIntroduzca el número de la opción deseada: ");
    scanf("%s", modo);
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
    double *ptr_entero = (double *)memoria;


    while (1)
    {   
        tBlock = clock();
        if (modo[0] == '2'){
            printf("Presiona Enter para continuar o Q para salir del programa...\n");
            int tecla = getchar(); // Leer el carácter introducido por el usuario
            if (tecla == '\n') {
                printf("Tomando un caracter nuevo!.\n");
            } else if (tecla == 'Q' || tecla == 'q') {
                ptr_entero[10] = 1;
                printf("El reconstructor se detendrá.\n");
                break;
            }
        }else if(modo[0] == '1'){
            sleep(1);
        }

        sem_wait(&semaforo);                                          // pedir semaforo
        tBlock_end = clock();
        ptr_entero[3] = ptr_entero[3] + ((double) (tBlock_end-tBlock)) / CLOCKS_PER_SEC;
                                                                    
        int caracteres_memoria = (int) ptr_entero[13];               // Acceder al carácter en el índice especificado
        if (caracteres_memoria > 0){
            int cont_reconstructor = (int) ptr_entero[14];
            if (cont_reconstructor < ptr_entero[12]-15){
                tk = clock();
                //Toma el caracter de memoria y lo guarda en el texto reconstruido
                caracter = (char) ptr_entero[16 + cont_reconstructor];
                writeToFile(caracter);
                ptr_entero[16 + cont_reconstructor] = 0;
                ptr_entero[13] = caracteres_memoria - 1;
                ptr_entero[14] = cont_reconstructor + 1;
                tk_end = clock();
                ptr_entero[9] = ptr_entero[9] + ((double) (tk_end-tk)) / CLOCKS_PER_SEC; // Segundos;
                
                tu = clock();
                //imprimir hora, campo, caracter y pos de memoria
                printf("El caracter introducido es: %c\n",caracter);            // imprimir caracter introducido
                time_t now = time(NULL);                                        // Obtener el tiempo actual
                struct tm *local = localtime(&now);                             // Convertir time_t a estructura tm como hora local
                printf("Hora de insersión en el texto reconstruido: %02d:%02d:%02d\n", local->tm_hour,  local->tm_min, local->tm_sec);  //horas:minutos:segundos
                int memPos = *(int *)memoria+cont_reconstructor;
                printf("Posición en memoria de donde se obtuvo el caracter: 0x%X\n\n", memPos); // imprimir posicion en memoria donde se inserta el caracter                
                tu_end = clock();
                ptr_entero[8] = ptr_entero[8] + ((double) (tu_end-tu)) / CLOCKS_PER_SEC; // Segundos;
                sem_post(&semaforo);
            }
            else{
                //Toma el caracter de memoria y lo guarda en el texto reconstruido
                tk = clock();
                caracter = (char) ptr_entero[16];
                writeToFile(caracter);
                ptr_entero[16] = 0;
                ptr_entero[13] = caracteres_memoria - 1;
                ptr_entero[14] = 1;    
                tk_end = clock();
                ptr_entero[9] = ptr_entero[9] + ((double) (tk_end-tk)) / CLOCKS_PER_SEC; // Segundos;
                
                tu = clock();
                //imprimir hora, campo, caracter y pos de memoria
                printf("El caracter introducido es: %c\n",caracter);            // imprimir caracter introducido
                time_t now = time(NULL);                                        // Obtener el tiempo actual
                struct tm *local = localtime(&now);                             // Convertir time_t a estructura tm como hora local
                printf("Hora de insersión en el texto reconstruido: %02d:%02d:%02d\n", local->tm_hour,  local->tm_min, local->tm_sec);  //horas:minutos:segundos
                int memPos = *(int *)memoria+cont_reconstructor;
                printf("Posición en memoria de donde se obtuvo el caracter: 0x%X\n\n", memPos); // imprimir posicion en memoria donde se inserta el caracter                
                tu_end = clock();
                ptr_entero[8] = ptr_entero[8] + ((double) (tu_end-tu)) / CLOCKS_PER_SEC; // Segundos;
                sem_post(&semaforo);
            }
        }else{
            sem_post(&semaforo);
            printf("No hay caracteres en memoria. \n\n");
        }                
        
        if (ptr_entero[1] == 1 && ptr_entero[13] == 0) 
        {   
            ptr_entero[10] = 1;
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