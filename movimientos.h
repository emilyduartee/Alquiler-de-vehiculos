#ifndef MOVIMIENTOS_H
#define MOVIMIENTOS_H
struct Pago {
	int idPago;
	int dniCliente;
	char patente[15];
	float monto;
	char fecha[11];
};
void submenuPagos();
void registrarPago();
void submenuReportes();
void listarPagos();
void calcRecaudacion();
#endif