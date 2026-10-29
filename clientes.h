#ifndef CLIENTES_H
#define CLIENTES_H

struct Cliente {
    long DNI;
    char apellidoNombre[50];
    char telefono[20];
    char email[50];
    char direccion[50];
    long fechaAlta;
    int estado;
};

void submenuClientes();
void registrarCliente();
void modificarCliente();
void buscarCliente();
int esFechaValida(long fecha);

#endif