#ifndef ALQUILERES_H
#define ALQUILERES_H

#include "comun.h"

struct Alquiler {
    int numAlquiler;
    int DNI;
    char patente[LON_PATENTE];
    int fechaInicio;
    int fechaPrevista;
    int fechaEfectiva;
    int cantDias;
    double importeTotal;
    double importePagado;
    char estado[LON_ESTADO];
};

void submenuAlquileres(void);
void realizarAlquiler(void);
void registrarDevolucion(void);
void listarAlquileres(void);
void modificarAlquiler(void);
void cancelarAlquiler(void);

/* Helpers de persistencia y utilidades */
int buscarAlquilerPorNumero(int num, struct Alquiler *out);
int buscarAlquilerActivoPorPatente(const char *pat, struct Alquiler *out);
int clienteTieneAlquilerActivo(int dni);
int vehiculoTieneAlquilerActivo(const char *pat);
int guardarAlquiler(const struct Alquiler *a);
int actualizarAlquiler(const struct Alquiler *a);
int proximoNumeroAlquiler(void);
void imprimirEncabezadoAlquileres(void);
void imprimirFilaAlquiler(const struct Alquiler *a);

#endif
