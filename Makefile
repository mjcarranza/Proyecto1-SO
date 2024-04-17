SOURCE1 = creador.c								# Archivo fuente creador
SOURCE2 = cliente.c								# Archivo fuente cliente
TARGET1 = creador_exe							# Ejecutable del creador
TARGET2 = cliente_exe							# Ejecutabla del cliente
CC = gcc										# Compilador a utilizar
CFLAGS = -Wall -Wextra -std=c99					# Opciones de compilación

all: $(TARGET1)	$(TARGET2)						# Regla para compilar el programa

# Regla para compilar el creador
$(TARGET1): $(SOURCE1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(SOURCE1)

# Regla para compilar el cliente
$(TARGET2): $(SOURCE2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(SOURCE2)

# Regla para ejecutar el creador
cr: $(TARGET1)
	./$(TARGET1)

# Regla para ejecutar el cliente
cl: $(TARGET2)
	./$(TARGET2)

# Regla para limpiar archivos basura
clean:
	rm -f $(TARGET1) $(TARGET2) creador cliente
