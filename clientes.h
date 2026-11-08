#ifndef CLIENTES_H
#define CLIENTES_H
#include "comun.h"

struct Cliente {
    int DNI;
    char apellidoNombre[LON_NOMBRE];
    char telefono[LON_TEL];
    char email[LON_EMAIL];
    char direccion[LON_DIR];
    int fechaAlta;
    int estado; /* 1 Activo, 0 Inactivo */
};

void submenuClientes(void);
void registrarCliente(void);
void modificarCliente(void);
void consultarCliente(void);
void buscarCliente(void);

/* Helpers de persistencia */
int buscarClientePorDNI(int dni, struct Cliente *out);
int existeCliente(int dni);
int guardarCliente(const struct Cliente *c);
int actualizarCliente(const struct Cliente *c);

#endif
