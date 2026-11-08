#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "movimientos.h"
#include "alquileres.h"
#include "clientes.h"
#include "comun.h"
#include "utils.h"
#include "fechas.h"

int proximoNumeroMovimiento(void) {
    FILE *f = fopen(ARCH_MOVIMIENTOS, "rb");
    if (!f) {
        return 1;
    }
    struct Movimiento m;
    int maxNum = 0;
    while (fread(&m, sizeof(struct Movimiento), 1, f) == 1) {
        if (m.numMovimiento > maxNum) {
            maxNum = m.numMovimiento;
        }
    }
    fclose(f);
    return maxNum + 1;
}

int regenerarArchivoMovimientosCliente(int dni) {
    char nombreTxt[64];
    sprintf(nombreTxt, "movimientos_%d.txt", dni);

    FILE *fMov = fopen(ARCH_MOVIMIENTOS, "rb");
    if (!fMov) {
        return 0;
    }

    FILE *fTxt = fopen(nombreTxt, "w");
    if (!fTxt) {
        printf("[ERROR] No se pudo crear o sobrescribir %s.\n", nombreTxt);
        fclose(fMov);
        return 0;
    }

    struct Movimiento m;
    int cantidad = 0;
    while (fread(&m, sizeof(struct Movimiento), 1, fMov) == 1) {
        if (m.DNI == dni) {
            /* Formato CSV ;: numMov;numAlq;Tipo;monto;fecha;DNI;patente;medio */
            fprintf(fTxt, "%d;%d;%s;%.2f;%d;%d;%s;%s\n",
                    m.numMovimiento,
                    m.numAlquiler,
                    m.tipoMov,
                    m.monto,
                    m.fecha,
                    m.DNI,
                    m.patente,
                    m.medioPago);
            cantidad++;
        }
    }
    fclose(fTxt);
    fclose(fMov);
    return 1;
}

int registrarMovimiento(int numAlq, const char *tipo, double monto, int fecha, const char *medio) {
    struct Alquiler a;
    if (!buscarAlquilerPorNumero(numAlq, &a)) {
        printf("[ERROR] No se encontro el alquiler Nro %d para registrar el movimiento.\n", numAlq);
        return 0;
    }

    struct Movimiento m;
    memset(&m, 0, sizeof(struct Movimiento));

    m.numMovimiento = proximoNumeroMovimiento();
    m.numAlquiler = numAlq;
    strncpy(m.tipoMov, tipo, LON_TIPO_MOV - 1);
    m.tipoMov[LON_TIPO_MOV - 1] = '\0';
    m.monto = monto;
    m.fecha = fecha;
    m.DNI = a.DNI;
    strncpy(m.patente, a.patente, LON_PATENTE - 1);
    m.patente[LON_PATENTE - 1] = '\0';
    strncpy(m.medioPago, medio ? medio : MEDIO_NINGUNO, LON_MEDIO_PAGO - 1);
    m.medioPago[LON_MEDIO_PAGO - 1] = '\0';

    FILE *f = fopen(ARCH_MOVIMIENTOS, "ab");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s para registrar el movimiento.\n", ARCH_MOVIMIENTOS);
        return 0;
    }
    size_t escritos = fwrite(&m, sizeof(struct Movimiento), 1, f);
    fclose(f);

    if (escritos != 1) {
        printf("[ERROR] Fallo al escribir el movimiento en %s.\n", ARCH_MOVIMIENTOS);
        return 0;
    }

    /* A16: Actualizar archivo individual de texto del cliente */
    regenerarArchivoMovimientosCliente(m.DNI);
    return 1;
}

void registrarPago(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("            REGISTRAR PAGO\n");
    printf("========================================\n");

    int numAlq = 0;
    leerEntero("Ingrese el numero de alquiler a pagar: ", 1, 9999999, &numAlq);

    struct Alquiler a;
    if (!buscarAlquilerPorNumero(numAlq, &a)) {
        printf("\n[ERROR] El alquiler Nro %d no existe en el sistema.\n", numAlq);
        pausarPantalla();
        return;
    }

    if (strcmp(a.estado, EST_ALQ_CANCELADO) == 0) {
        printf("\n[ERROR] No se pueden registrar pagos para alquileres CANCELADOS.\n");
        pausarPantalla();
        return;
    }

    double saldoPendiente = a.importeTotal - a.importePagado;
    if (saldoPendiente <= TOLERANCIA_SALDO) {
        printf("\n[AVISO] El alquiler Nro %d ya se encuentra totalmente saldado.\n", numAlq);
        printf("Importe Total:  $%.2f\n", a.importeTotal);
        printf("Importe Pagado: $%.2f\n", a.importePagado);
        pausarPantalla();
        return;
    }

    printf("\n--- DATOS DEL ALQUILER ---\n");
    printf("Nro Alquiler:    %d\n", a.numAlquiler);
    printf("DNI Cliente:     %d\n", a.DNI);
    printf("Patente Vehic:   %s\n", a.patente);
    printf("Fecha de Inicio: %d\n", a.fechaInicio);
    printf("Importe Total:   $%.2f\n", a.importeTotal);
    printf("Importe Pagado:  $%.2f\n", a.importePagado);
    printf("SALDO PENDIENTE: $%.2f\n", saldoPendiente);
    printf("---------------------------\n");

    double monto = 0.0;
    while (1) {
        leerDouble("Ingrese el monto a pagar: ", 0.0, &monto);
        if (monto > saldoPendiente + TOLERANCIA_SALDO) {
            printf("[ERROR] El monto no puede superar el saldo pendiente ($%.2f). Reintente.\n",
                   saldoPendiente);
            continue;
        }
        break;
    }

    printf("\nSeleccione medio de pago:\n");
    printf("1. %s\n", MEDIO_EFECTIVO);
    printf("2. %s\n", MEDIO_TARJETA);
    printf("3. %s\n", MEDIO_TRANSFERENCIA);
    int opcMedio = 1;
    leerEntero("Opcion: ", 1, 3, &opcMedio);
    const char *medioElegido = MEDIO_EFECTIVO;
    if (opcMedio == 2) medioElegido = MEDIO_TARJETA;
    else if (opcMedio == 3) medioElegido = MEDIO_TRANSFERENCIA;

    int fechaPago = 0;
    while (1) {
        leerFecha("Ingrese la fecha del pago (AAAAMMDD): ", &fechaPago);
        if (fechaPago < a.fechaInicio) {
            printf("[ERROR] La fecha de pago no puede ser anterior al inicio del alquiler (%d). Reintente.\n",
                   a.fechaInicio);
            continue;
        }
        break;
    }

    /* Actualizar alquiler primero */
    a.importePagado += monto;
    if (!actualizarAlquiler(&a)) {
        printf("\n[ERROR] No se pudo actualizar el importe pagado en alquileres.dat.\n");
        pausarPantalla();
        return;
    }

    /* Registrar movimiento */
    if (registrarMovimiento(a.numAlquiler, MOV_PAGO, monto, fechaPago, medioElegido)) {
        printf("\n========================================\n");
        printf(">>> PAGO REGISTRADO EXITOSAMENTE!\n");
        printf("Alquiler Nro:          %d\n", a.numAlquiler);
        printf("Monto Abonado:         $%.2f\n", monto);
        printf("Medio de Pago:         %s\n", medioElegido);
        printf("Nuevo Saldo Pendiente: $%.2f\n", a.importeTotal - a.importePagado);
        printf("========================================\n");
    } else {
        printf("\n[ERROR] Se actualizo el alquiler pero ocurrio un error al registrar el movimiento.\n");
    }
    pausarPantalla();
}

void listarMovimientos(void) {
    limpiarPantalla();
    printf("\n========================================================================================\n");
    printf("                        LISTADO GENERAL DE MOVIMIENTOS\n");
    printf("========================================================================================\n");

    FILE *f = fopen(ARCH_MOVIMIENTOS, "rb");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s o no hay movimientos registrados.\n", ARCH_MOVIMIENTOS);
        pausarPantalla();
        return;
    }

    printf("%-6s | %-6s | %-8s | %-12s | %-10s | %-10s | %-10s | %-14s\n",
           "NRO", "ALQ", "TIPO", "MONTO", "FECHA", "DNI", "PATENTE", "MEDIO");
    printf("----------------------------------------------------------------------------------------\n");

    struct Movimiento m;
    int cont = 0;
    double totalMontos = 0.0;
    while (fread(&m, sizeof(struct Movimiento), 1, f) == 1) {
        printf("%-6d | %-6d | %-8s | $%11.2f | %-10d | %-10d | %-10s | %-14s\n",
               m.numMovimiento, m.numAlquiler, m.tipoMov, m.monto, m.fecha,
               m.DNI, m.patente, m.medioPago);
        if (strcmp(m.tipoMov, MOV_PAGO) == 0) {
            totalMontos += m.monto;
        }
        cont++;
    }
    fclose(f);

    if (cont == 0) {
        printf("No hay movimientos registrados.\n");
    } else {
        printf("\nTotal de movimientos: %d (Suma total de Pagos: $%.2f)\n", cont, totalMontos);
    }
    pausarPantalla();
}

void submenuMovimientos(void) {
    int opc = -1;
    do {
        limpiarPantalla();
        printf("\n========================================\n");
        printf("           SUBMENU PAGOS\n");
        printf("========================================\n");
        printf("1. Registrar pago\n");
        printf("2. Listar todos los movimientos\n");
        printf("0. Volver al Menu Principal\n");
        printf("========================================\n");
        leerEntero("Seleccione una opcion: ", 0, 2, &opc);

        switch (opc) {
            case 1:
                registrarPago();
                break;
            case 2:
                listarMovimientos();
                break;
            case 0:
                break;
            default:
                printf("\n[ERROR] Opcion no valida.\n");
                pausarPantalla();
                break;
        }
    } while (opc != 0);
}
