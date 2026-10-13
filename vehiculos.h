#ifndef VEHICULOS_H
#define VEHICULOS_H
struct Vehiculo{
    char patente[15];
    char marca[30];
    char modelo[30];
    int anio;
    char tipo[30];
    int capacidad;
    float precio_dia;
    char estado[20];
    };
void submenuVehiculos();
void registrarVehiculo();
void modificarVehiculo();
void buscarVehiculo();
#endif