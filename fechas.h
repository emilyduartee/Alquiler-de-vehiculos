#ifndef FECHAS_H
#define FECHAS_H

/* =========================================================================
   Modulo de Fechas y Calendario Civil
   Manejo de fechas en formato numerico AAAAMMDD
   ========================================================================= */

int esBisiesto(int anio);
int diasEnMes(int anio, int mes);
int esFechaValida(int f);
int fechaADias(int f);
int diasAFecha(int n);
int diasEntre(int f1, int f2);
int sumarDias(int f, int n);
int fechaActual(void);

#endif
