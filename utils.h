#ifndef UTILS_H
#define UTILS_H

/* =========================================================================
   Modulo de Utilidades de Consola y Validacion de Entrada
   ========================================================================= */

void limpiarBuffer(void);
void limpiarPantalla(void);
void pausarPantalla(void);

int leerEntero(const char *msg, int min, int max, int *out);
int leerDouble(const char *msg, double minExclusivo, double *out);
int leerCadena(const char *msg, char *dest, int tam, int permitirVacio);
int leerDNI(const char *msg, int *dni);
int leerPatente(const char *msg, char *dest);
int leerFecha(const char *msg, int *fecha);
int leerSN(const char *msg);

#endif
