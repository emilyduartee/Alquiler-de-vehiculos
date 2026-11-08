#ifndef MOVIMIENTOS_H
#define MOVIMIENTOS_H

#include "comun.h"

struct Movimiento {
    int numMovimiento;
    int numAlquiler;
    char tipoMov[LON_TIPO_MOV]; /* "Pago" | "Ajuste" */
    double monto;
    int fecha;
    int DNI;
    char patente[LON_PATENTE];
    char medioPago[LON_MEDIO_PAGO];
};

void submenuMovimientos(void);
void registrarPago(void);
void listarMovimientos(void);
int proximoNumeroMovimiento(void);
int registrarMovimiento(int numAlq, const char *tipo, double monto, int fecha, const char *medio);
int regenerarArchivoMovimientosCliente(int dni);

#endif
