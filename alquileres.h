#ifndef ALQUILERES_H
#define ALQUILERES_H

struct Alquiler {
    int numAlquiler;
    long DNI;
    char patente[15];
    long fechaInicio;
    long fechaPrevista;
    long fechaEfectiva;
    int cantDias;
    float importeTotal;
    float importePagado;
    char estado[20];
};

void submenuAlquileres();
void realizarAlquiler();
void registrarDevolucion();
void listarAlquileres();
void modificarAlquiler();

#endif