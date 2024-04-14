SOURCE = creador.c								# Nombre del archivo fuente .c
TARGET = creador_exe							# Nombre del ejecutable generado
CC = gcc										# Compilador a utilizar
CFLAGS = -Wall -Wextra -std=c99					# Opciones de compilación

all: $(TARGET)									# Regla para compilar el programa

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

run: $(TARGET)									# Regla para ejecutar el programa
	./$(TARGET)

clean:											# Regla para limpiar los archivos generados
	rm -f $(TARGET) creador
