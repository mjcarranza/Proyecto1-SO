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
#define MAX_SIZE 10000  // Tamaño máximo del archivo

char buffer[MAX_SIZE];
char caracter;          // variable para guardar el caracter leido
sem_t semaforo;         // instancia de semaforo
size_t bytesLeidos;     // longitud del buffer
int contador;           // contador para conrolar los espacios de memoria donde se escribe
int *ptr_entero;        // puntero para la memoria compartida
double tUser = 0.0, tKernel = 0.0, tBlocked = 0.0;

int main() {

    FILE *archivo;
    clock_t tu, tu_end, tk, tk_end, tBlock, tBlock_end; 
    
    /*----------------Pedir al usuario el nombre del archivo----------------*/
    char direccion[50] = "archivos/";
    char nombre[50]; // Variable para almacenar el nombre del archivo
    char modo[1];
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

    if (modo[0] != '1'){
        if (modo[0] != '2')
        {
            printf("La opción elegida no es válida. Saliendo...");

            if (shmdt(memoria) == -1) {
                perror("Error al desadjuntar la memoria compartida");
                exit(1);
            }
            fclose(archivo);        // Cierra el archivo 
            return 0;
        }
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

    /* --------------------------- Bucle de ejecucion ----------------------------*/
    while (1)
    {   
        tu = clock();
        if (modo[0] == '2'){
            // esperar un enter
            printf("Presione 'Enter' para tomar otro caracter!\n");
            getchar();
        }
        tu_end = clock();
        tUser += tu_end - tu;

        tBlock = clock();
        sem_wait(&semaforo);                                                    // pedir semaforo
        tBlock_end = clock();
        tBlocked += tBlock_end-tBlock;
        
        if (ptr_entero[0] < bytesLeidos){                                       // verificar si ya se termino de leer el archivo
        
            tk = clock();
            contador = ptr_entero[15];
            tk_end = clock();
            tKernel += tk_end-tk; 

            // ptr[12] - 15 = numero de caracteres que se comparten
            if (ptr_entero[13] <= (ptr_entero[12]-15))
            {
                if (contador <= ptr_entero[12])                                     // si se cumple, se escribe (memoria compartida para caracteres empieza en posicion 1 ==> contador = 1)
                {
                    tk = clock();
                    caracter = buffer[ptr_entero[0]];                               // Acceder al carácter en el índice especificado
                    ptr_entero[contador] = caracter;                                // escribir en al direccion de memoria compartida
                    ptr_entero[13] = ptr_entero[13] + 1;                            // sumar a la cantidad de caracteres en memoria
                    ptr_entero[5] = ptr_entero[5] + sizeof(caracter);
                    ptr_entero[0] = ptr_entero[0] + 1;                              // aumentar la posicion en la que se debe leer del buffer
                    ptr_entero[15] = ptr_entero[15] + 1;                            // aumentar el contador
                    tk_end = clock();

                    tu = clock();
                    tKernel += tk_end-tk; 
                    
                    printf("El caracter introducido es: %c\n",caracter);            // imprimir caracter introducido
                    time_t now = time(NULL);                                        // Obtener el tiempo actual
                    struct tm *local = localtime(&now);                             // Convertir time_t a estructura tm como hora local
                    printf("Hora de insersión en memoria: %02d:%02d:%02d\n", local->tm_hour,  local->tm_min, local->tm_sec);  //horas:minutos:segundos
                    int memPos = (int *)memoria+contador;
                    printf("Posición en memoria donde se introdujo el caracter: 0x%X\n\n", memPos); // imprimir posicion en memoria donde se inserta el caracter                
                    printf("El contador es: %d\n", ptr_entero[0]);
                    printf("El contador de for es: %d\n", ptr_entero[15]);
                    tu_end = clock();
                    tUser = tu_end - tu;
                    //sleep(2);
                    
                }
            }
            

            
            else{                                                               // volver al inicio de la memoria compartida para los caracteres
                sleep(1);
                tk = clock();
                ptr_entero[15] = 16;
                tk_end = clock();
                tKernel += tk_end-tk; 
            }            
        }

        else{
            ptr_entero[7] = ((double) (tKernel)) / CLOCKS_PER_SEC; // Segundos;
            ptr_entero[6] = ((double) (tUser)) / CLOCKS_PER_SEC; // Segundos;
            ptr_entero[2] = ((double) (tBlocked)) / CLOCKS_PER_SEC;
            ptr_entero[1] = 1;
            printf("Se acabo el documento \n");
            sem_post(&semaforo);                                                // liberar el semaforo
            break;
        }
        sem_post(&semaforo);                                                    // liberar el semaforo
    }

    
    /*----------------------Desadjuntar la memoria compartida--------------------------*/
    if (shmdt(memoria) == -1) {
        perror("Error al desadjuntar la memoria compartida");
        exit(1);
    }

    fclose(archivo);    // Cierra el archivo 
    return 0;
}