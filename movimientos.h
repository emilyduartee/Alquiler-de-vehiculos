#ifndef MOVIMIENTOS_H
#define MOVIMIENTOS_H

struct Movimiento {
    int numMovimiento;
    int numAlquiler;
    long DNI;
    long fecha;
    float monto;
    char tipoPago[20];
};

void submenuMovimientos();
void registrarPago();
void listarMovimientos();
void guardarEnTextoCliente(struct Movimiento mov, float saldoRestante);

#endif