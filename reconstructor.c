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
    ptr_entero = (int *)memoria;    // ver si debo poner el asterisco antes de ptr


    while (1)
    {   
        if (modo[0] == '2'){
            tBlock = clock();
            printf("Presiona Enter para continuar...\n");
            int tecla = getchar(); // Leer el carácter introducido por el usuario
            if (tecla == '\n') {
                sem_wait(&semaforo);  
                tBlock_end = clock();
                ptr_entero[3] = ptr_entero[3] + ((double) (tBlock_end-tBlock)) / CLOCKS_PER_SEC;
                                                                // pedir semaforo
                int caracteres_memoria = (int) ptr_entero[13];               // Acceder al carácter en el índice especificado

                if (caracteres_memoria > 0){
                    int cont_reconstructor = (int) ptr_entero[14];
                    if (cont_reconstructor < MEM_SIZE){
                        tk = clock();
                        caracter = (char) ptr_entero[16 + cont_reconstructor];
                        writeToFile(caracter);
                        ptr_entero[16 + cont_reconstructor] = 0;
                        ptr_entero[13] = caracteres_memoria - 1;
                        ptr_entero[14] = cont_reconstructor + 1;
                        tk_end = clock();
                        ptr_entero[9] = ptr_entero[9] + ((double) (tk_end-tk)) / CLOCKS_PER_SEC; // Segundos;
                        sem_post(&semaforo);
                    }
                    else{
                        tk = clock();
                        caracter = (char) ptr_entero[16];
                        writeToFile(caracter);
                        ptr_entero[16] = 0;
                        ptr_entero[13] = caracteres_memoria - 1;
                        ptr_entero[14] = 1;    
                        tk_end = clock();
                        ptr_entero[9] = ptr_entero[9] + ((double) (tk_end-tk)) / CLOCKS_PER_SEC; // Segundos;
                        sem_post(&semaforo);
                    }
                }else{
                    sem_post(&semaforo);
                    printf("No hay caracteres en memoria");
                }
            } else if (tecla == 'Q' || tecla == 'q') {
                printf("El programa terminará.\n");
            }
            
        }else if(modo[0] == '1'){
            tBlock = clock();
            sem_wait(&semaforo);  
            tBlock_end = clock();
            ptr_entero[3] = ptr_entero[3] + ((double) (tBlock_end-tBlock)) / CLOCKS_PER_SEC;
                                                                // pedir semaforo
            int caracteres_memoria = (int) ptr_entero[13];               // Acceder al carácter en el índice especificado

            if (caracteres_memoria > 0){
                int cont_reconstructor = (int) ptr_entero[14];
                if (cont_reconstructor < MEM_SIZE){
                    tk = clock();
                    caracter = (char) ptr_entero[16 + cont_reconstructor];
                    writeToFile(caracter);
                    ptr_entero[16 + cont_reconstructor] = 0;
                    ptr_entero[13] = caracteres_memoria - 1;
                    ptr_entero[14] = cont_reconstructor + 1;
                    tk_end = clock();
                    ptr_entero[9] = ptr_entero[9] + ((double) (tk_end-tk)) / CLOCKS_PER_SEC; // Segundos;
                    sem_post(&semaforo);
                    sleep(4);
                }
                else{
                    tk = clock();
                    caracter = (char) ptr_entero[16];
                    writeToFile(caracter);
                    ptr_entero[16] = 0;
                    ptr_entero[13] = caracteres_memoria - 1;
                    ptr_entero[14] = 1;    
                    tk_end = clock();
                    ptr_entero[9] = ptr_entero[9] + ((double) (tk_end-tk)) / CLOCKS_PER_SEC; // Segundos;
                    sem_post(&semaforo);
                    sleep(4);
                }
            }else{
                sem_post(&semaforo);
                printf("No hay caracteres en memoria");
                sleep(4);
            }        
        }
        
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