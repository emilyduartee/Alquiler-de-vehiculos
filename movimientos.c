#include <stdio.h>
#include <stdlib.h>
#include "movimientos.h"
#include "menu.h"
void submenuPagos(){
	int opc;
	do{
		system("cls");
		printf("\n===GESTION DE PAGOS===\n");
		printf("1. Registrar pago");
		printf("0. Volver al menu principal");
		printf("Ingrese una opcion: ");
		scanf("%d", &opc);
		switch (opc) {
			case 1:
				system("cls");
				registrarPago();
				break;
			case 0:
				break;
			default:
				system("cls");
                printf("\n>>> ERROR: Opcion no valida.\n");
                break;
        }
	} while (opc != 0);
}

void registrarPago(){
	struct Pago nuevoPago;
    FILE *archivo;
    system("cls");
    printf("\n--- REGISTRAR PAGO ---\n");
    // el ID de recibo o monto no puede ser negativo
    do {
        printf("Ingrese el numero de recibo (mayor a 0): ");
        scanf("%d", &nuevoPago.idPago);
        if (nuevoPago.idPago <= 0) {
            printf("[ERROR] El numero de recibo debe ser un valor positivo.\n");
        }
    } while (nuevoPago.idPago <= 0);
    printf("Ingrese el DNI del cliente: ");
    scanf("%d", &nuevoPago.dniCliente);
    printf("Ingrese la patente del vehiculo: ");
    scanf("%s", nuevoPago.patente);
    printf("Ingrese el monto cobrado: $");
    scanf("%f", &nuevoPago.monto);
    printf("Ingrese la fecha (DD/MM/AAAA): ");
    scanf("%s", nuevoPago.fecha);
    archivo = fopen("movimientos.dat", "ab");
    if (archivo != NULL) {
        fwrite(&nuevoPago, sizeof(struct Pago), 1, archivo);
        fclose(archivo);
        printf("\n>>> Pago registrado exitosamente!\n");
    } else {
        printf("\n[ERROR] No se pudo acceder a la base de datos de movimientos.\n");
    }
    pausarPantalla();
}

void submenuReportes() {
    int opc;
    do {
        system("cls");
        printf("\n=== CONSULTAS Y REPORTES ===\n");
        printf("1. Listar historial de pagos\n");
        printf("2. Calcular recaudacion total\n");
        printf("0. Volver al Menu Principal\n");
        printf("Ingrese una opcion: ");
        scanf("%d", &opc);

        switch(opc) {
            case 1:
                listarPagos();
                break;
            case 2:
                calcularRecaudacion();
                break;
            case 0:
                break;
            default:
                printf("\n>>> ERROR: Opcion no valida.\n");
                pausarPantalla();
                break;
        }
    } while (opc != 0);
}

void listarPagos() {
    struct Pago pagoLeido;
    FILE *archivo;
    int contador = 0; // para saber si al menos se encuentra 1 pago
    system("cls");
    printf("\n--- HISTORIAL DE PAGOS ---\n");
    // para evitar crear un archivo vacio si no existe
    archivo = fopen("movimientos.dat", "rb");
    if (archivo == NULL) {
        printf("\n[INFO] No hay transacciones registradas en el sistema.\n");
        pausarPantalla();
        return;
    }
    while (fread(&pagoLeido, sizeof(struct Pago), 1, archivo)) {
        contador++;
        printf("\nRecibo Nro: %d | Fecha: %s\n", pagoLeido.idPago, pagoLeido.fecha);
        printf("DNI: %d | Patente: %s\n", pagoLeido.dniCliente, pagoLeido.patente);
        printf("MONTO: $%.2f\n", pagoLeido.monto);
        printf("--------------------------------------\n");
    }
    if (contador == 0) {
        printf("\n[INFO] El archivo contable esta vacio.\n");
    }
    fclose(archivo);
    pausarPantalla();
}

void calcularRecaudacion() {
    struct Pago pagoLeido;
    FILE *archivo;
    float totalRecaudado = 0.0;
    system("cls");
    printf("\n--- RECAUDACION TOTAL ---\n");
    archivo = fopen("movimientos.dat", "rb");
    if (archivo == NULL) {
        printf("\nRecaudacion total del sistema: $0.00\n");
        pausarPantalla();
        return;
    }
    // El while itera sobre cada pago guardado y suma el flotante al total
    while (fread(&pagoLeido, sizeof(struct Pago), 1, archivo)) {
        totalRecaudado = totalRecaudado + pagoLeido.monto;
    }
    printf("\n========================================\n");
    printf("  RECAUDACION TOTAL: $%.2f\n", totalRecaudado);
    printf("========================================\n");
    fclose(archivo);
    pausarPantalla();
}