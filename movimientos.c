#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "movimientos.h"
#include "menu.h"

void submenuPagos() {
    int opc;
    do {
        system("cls");
        printf("\n=== GESTION DE PAGOS ===\n");
        printf("1. Registrar nuevo pago\n");
        printf("0. Volver al Menu Principal\n");
        printf("Ingrese una opcion: ");
        scanf("%d", &opc);
        switch(opc) {
            case 1:
                registrarPago();
                break;
            case 0:
                printf("\n>>> Volviendo al Menu Principal...\n");
                break;
            default:
                printf("\n>>> ERROR: Opcion no valida.\n");
                pausarPantalla();
                break;
        }
    } while (opc != 0);
}

void registrarPago() {
    struct Movimiento nuevoPago;
    FILE *archivoBinario;
    FILE *archivoTexto;
    char nombreArchivoCliente[50];
    system("cls");
    printf("\n--- REGISTRAR NUEVO PAGO ---\n");
    printf("Ingrese el numero de movimiento (comprobante): ");
    scanf("%d", &nuevoPago.numero_movimiento);
    printf("Ingrese el numero de alquiler asociado: ");
    scanf("%d", &nuevoPago.numero_alquiler);
    printf("Ingrese el tipo de movimiento (Pago o Ajuste): ");
    fflush(stdin);
    gets(nuevoPago.tipo_movimiento);
    printf("Ingrese el monto: $");
    scanf("%f", &nuevoPago.monto);
    printf("Ingrese la fecha (DD/MM/AAAA): ");
    scanf("%s", nuevoPago.fecha);
    printf("Ingrese el DNI del cliente: ");
    scanf("%d", &nuevoPago.dni_cliente);
    printf("Ingrese la patente del vehiculo: ");
    gets(nuevoPago.patente);
    archivoBinario = fopen("movimientos.dat", "ab");
    if (archivoBinario != NULL) {
        fwrite(&nuevoPago, sizeof(struct Movimiento), 1, archivoBinario);
        fclose(archivoBinario);
        printf("\n>>> Pago registrado exitosamente en movimientos.dat\n");
    } else {
        printf("\n[ERROR] No se pudo abrir movimientos.dat\n");
    }
    // archivo dinamico usando el DNI del cliente
    sprintf(nombreArchivoCliente, "movimientos_%d.txt", nuevoPago.dni_cliente);
    archivoTexto = fopen(nombreArchivoCliente, "a");
    if (archivoTexto != NULL) {
        fprintf(archivoTexto, "Mov: %d | Alquiler: %d | Tipo: %s | Monto: $%.2f | Fecha: %s | Patente: %s\n",
                nuevoPago.numero_movimiento,
                nuevoPago.numero_alquiler,
                nuevoPago.tipo_movimiento,
                nuevoPago.monto,
                nuevoPago.fecha,
                nuevoPago.patente);
        fclose(archivoTexto);
        printf(">>> Historial del cliente actualizado en %s\n", nombreArchivoCliente);
    } else {
        printf("[ERROR] No se pudo actualizar el archivo de texto del cliente.\n");
    }
    pausarPantalla();
}