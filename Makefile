SOURCE1 = creador.c								# Archivo fuente creador
SOURCE2 = cliente.c								# Archivo fuente cliente
SOURCE3 = estadisticas.c						# Archivo fuente estadisticas

TARGET1 = creador_exe							# Ejecutable del creador
TARGET2 = cliente_exe							# Ejecutable del cliente
TARGET3 = estadisticas_exe						# ejecutable del reconstructor

CC = gcc										# Compilador a utilizar
CFLAGS = -Wall -Wextra -std=c99					# Opciones de compilación

all: $(TARGET1)	$(TARGET2) $(TARGET3)			# Regla para compilar el programa

# Regla para compilar el creador
$(TARGET1): $(SOURCE1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(SOURCE1)

# Regla para compilar el cliente
$(TARGET2): $(SOURCE2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(SOURCE2)

# Regla para compilar estadistica
$(TARGET3): $(SOURCE3)
	$(CC) $(CFLAGS) -o $(TARGET3) $(SOURCE3)

# Regla para ejecutar el creador
cr: $(TARGET1)
	./$(TARGET1)

# Regla para ejecutar el cliente
cl: $(TARGET2)
	./$(TARGET2)

# Regla para ejecutar las estadisticas
est: $(TARGET3)
	./$(TARGET3)

# Regla para limpiar archivos basura
clean:
	rm -f $(TARGET1) $(TARGET2) $(TARGET3) creador cliente estadisticas
