#ifndef MOVIMIENTOS_H
#define MOVIMIENTOS_H

struct Movimiento {
    int numero_movimiento;
    int numero_alquiler;
    char tipo_movimiento[20]; // pago o Ajuste
    float monto;
    char fecha[11];
    int dni_cliente;
    char patente[15];
};
void submenuPagos();
void registrarPago();
#endif