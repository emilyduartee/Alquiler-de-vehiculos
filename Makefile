# Makefile portable para Sistema de Gestion de Alquiler de Vehiculos
CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 -O2
SRCS = main.c menu.c clientes.c vehiculos.c alquileres.c movimientos.c reportes.c utils.c fechas.c
OBJS = $(SRCS:.c=.o)
TARGET = alquiler.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
