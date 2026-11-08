#ifndef VEHICULOS_H
#define VEHICULOS_H

#include "comun.h"

struct Vehiculo {
    char patente[LON_PATENTE];
    char marca[LON_MARCA];
    char modelo[LON_MODELO];
    int anio;
    char tipo[LON_TIPO_VEH];
    int capacidad;
    double precio_dia;
    char estado[LON_ESTADO];
};

void submenuVehiculos(void);
void registrarVehiculo(void);
void modificarVehiculo(void);
void buscarVehiculo(void);
void listarVehiculos(void);

/* Helpers de persistencia */
int buscarVehiculoPorPatente(const char *pat, struct Vehiculo *out);
int existeVehiculo(const char *pat);
int guardarVehiculo(const struct Vehiculo *v);
int actualizarVehiculo(const struct Vehiculo *v);
int actualizarEstadoVehiculo(const char *pat, const char *estado);

#endif
