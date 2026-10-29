#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "movimientos.h"
#include "alquileres.h"
#include "clientes.h"
#include "menu.h"

void submenuMovimientos() {
    int opc;
    do {
        system("cls");
        printf("\n=== SUBMENU GESTION DE PAGOS Y MOVIMIENTOS ===\n");
        printf("1. Registrar pago / cobro de alquiler\n");
        printf("2. Listar historial general de pagos\n");
        printf("0. Volver al Menu Principal\n");
        printf("Ingrese una opcion: ");
        scanf("%d", &opc);

        switch(opc) {
            case 1:
                system("cls");
                registrarPago();
                pausarPantalla();
                break;
            case 2:
                system("cls");
                listarMovimientos();
                pausarPantalla();
                break;
            case 0:
                system("cls");
                printf("\n>>> Volviendo al Menu Principal...\n");
                break;
            default:
                system("cls");
                printf("\n>>> ERROR: Opcion no valida.\n");
                pausarPantalla();
                break;
        }
    } while (opc != 0);
}

void registrarPago() {
    FILE *fAlquileres, *fMov;
    struct Alquiler alq;
    struct Movimiento nuevoMov, auxMov;
    int numAlqBuscado;
    int encontrado = 0;
    int ultimoNumMov = 0;
    float saldoPendiente = 0.0;
    int opcMedio;
    system("cls");
    printf("\n--- REGISTRAR PAGO DE ALQUILER ---\n");
    do {
        printf("Ingrese el Numero de Alquiler a cobrar: ");
        if (scanf("%d", &numAlqBuscado) != 1) {
            printf("[ERROR] Ingrese un numero entero valido. Reintente.\n");
            fflush(stdin);
            numAlqBuscado = 0;
        }
    } while (numAlqBuscado <= 0);
    fAlquileres = fopen("alquileres.dat", "rb+");
    if (fAlquileres == NULL) {
        printf("[ERROR] No se pudo abrir el archivo alquileres.dat o no hay registros.\n");
        return;
    }
    while (fread(&alq, sizeof(struct Alquiler), 1, fAlquileres) == 1) {
        if (alq.numAlquiler == numAlqBuscado) {
            encontrado = 1;
            saldoPendiente = alq.importeTotal - alq.importePagado;
            printf("\nAlquiler Nro: %d | DNI Cliente: %ld | Patente: %s\n", 
                   alq.numAlquiler, alq.DNI, alq.patente);
            printf("Importe Total: $%.2f | Acumulado Pagado: $%.2f\n", 
                   alq.importeTotal, alq.importePagado);
            printf("SALDO PENDIENTE: $%.2f\n", saldoPendiente);
            if (saldoPendiente <= 0.001) {
                printf("\n[AVISO] Este alquiler ya se encuentra totalmente saldado.\n");
                fclose(fAlquileres);
                return;
            }
            nuevoMov.numAlquiler = alq.numAlquiler;
            nuevoMov.DNI = alq.DNI;
            do {
                printf("\nIngrese la fecha del pago (AAAAMMDD): ");
                scanf("%ld", &nuevoMov.fecha);
                if (!esFechaValida(nuevoMov.fecha)) {
                    printf("[ERROR] Fecha invalida. Reintente.\n");
                }
            } while (!esFechaValida(nuevoMov.fecha));
            do {
                printf("Ingrese el monto a abonar (Maximo $%.2f): $", saldoPendiente);
                if (scanf("%f", &nuevoMov.monto) != 1) {
                    printf("[ERROR] Ingrese un valor numerico valido.\n");
                    while (getchar() != '\n');
                    nuevoMov.monto = 0.0;
                } else if (nuevoMov.monto <= 0) {
                    printf("[ERROR] El monto debe ser mayor a cero.\n");
                } else if (nuevoMov.monto > saldoPendiente) {
                    printf("[ERROR] El monto ingresado supera el saldo pendiente ($%.2f).\n", saldoPendiente);
                }
            } while (nuevoMov.monto <= 0 || nuevoMov.monto > saldoPendiente);
            printf("\nSeleccione medio de pago:\n");
            printf("1. Efectivo\n");
            printf("2. Tarjeta\n");
            printf("3. Transferencia\n");
            printf("Opcion: ");
            scanf("%d", &opcMedio);
            switch(opcMedio) {
                case 1: strcpy(nuevoMov.tipoPago, "Efectivo"); break;
                case 2: strcpy(nuevoMov.tipoPago, "Tarjeta"); break;
                case 3: strcpy(nuevoMov.tipoPago, "Transferencia"); break;
                default: strcpy(nuevoMov.tipoPago, "Efectivo"); break;
            }

            // Asignacion de ID autoincremental de movimiento
            fMov = fopen("movimientos.dat", "rb");
            if (fMov != NULL) {
                while (fread(&auxMov, sizeof(struct Movimiento), 1, fMov) == 1) {
                    ultimoNumMov = auxMov.numMovimiento;
                }
                fclose(fMov);
            }
            nuevoMov.numMovimiento = ultimoNumMov + 1;

            // Actualizacion del archivo binario alquileres.dat
            alq.importePagado += nuevoMov.monto;
            fseek(fAlquileres, -sizeof(struct Alquiler), SEEK_CUR);
            fwrite(&alq, sizeof(struct Alquiler), 1, fAlquileres);
            fclose(fAlquileres);

            // Guardar en el binario general de movimientos
            fMov = fopen("movimientos.dat", "ab");
            if (fMov != NULL) {
                fwrite(&nuevoMov, sizeof(struct Movimiento), 1, fMov);
                fclose(fMov);
            }

            // Generacion y actualizacion del archivo .txt individual del cliente
            float saldoRestantePosterior = alq.importeTotal - alq.importePagado;
            guardarEnTextoCliente(nuevoMov, saldoRestantePosterior);

            printf("\n==================================================\n");
            printf(">>> PAGO REGISTRADO EXITOSAMENTE!\n");
            printf("Comprobante Nro:    %d\n", nuevoMov.numMovimiento);
            printf("Monto Abonado:      $%.2f (%s)\n", nuevoMov.monto, nuevoMov.tipoPago);
            printf("Saldo Restante:     $%.2f\n", saldoRestantePosterior);
            printf("==================================================\n");
            break;
        }
    }

    if (!encontrado) {
        printf("[ERROR] No se encontro el alquiler Numero %d.\n", numAlqBuscado);
        fclose(fAlquileres);
    }
}

void guardarEnTextoCliente(struct Movimiento mov, float saldoRestante) {
    FILE *archTxt;
    char nombreArchivo[40];

    // Creacion dinámica del nombre: movimientos_DNI.txt
    sprintf(nombreArchivo, "movimientos_%ld.txt", mov.DNI);

    archTxt = fopen(nombreArchivo, "a");
    if (archTxt != NULL) {
        fprintf(archTxt, "MOVILIDAD Nro: %d | Alquiler: %d | Fecha: %ld | Pago: $%.2f (%s) | Saldo Pendiente: $%.2f\n",
                mov.numMovimiento, mov.numAlquiler, mov.fecha, mov.monto, mov.tipoPago, saldoRestante);
        fclose(archTxt);
    } else {
        printf("[ERROR] No se pudo actualizar el archivo de texto del cliente (%s).\n", nombreArchivo);
    }
}

void listarMovimientos() {
    FILE *archivo;
    struct Movimiento mov;
    int cont = 0;

    system("cls");
    printf("\n--- HISTORIAL GENERAL DE MOVIMIENTOS Y COBROS ---\n");
    archivo = fopen("movimientos.dat", "rb");

    if (archivo == NULL) {
        printf("[ERROR] No hay movimientos registrados en el sistema.\n");
        return;
    }

    printf("%-8s %-10s %-10s %-10s %-12s %-15s\n", 
           "NRO MOV", "NRO ALQ", "DNI", "FECHA", "MONTO", "MEDIO PAGO");
    printf("-------------------------------------------------------------------\n");

    while (fread(&mov, sizeof(struct Movimiento), 1, archivo) == 1) {
        printf("%-8d %-10d %-10ld %-10ld $%-11.2f %-15s\n",
               mov.numMovimiento,
               mov.numAlquiler,
               mov.DNI,
               mov.fecha,
               mov.monto,
               mov.tipoPago);
        cont++;
    }

    if (cont == 0) {
        printf("El archivo de movimientos esta vacio.\n");
    } else {
        printf("\nTotal de pagos registrados: %d\n", cont);
    }

    fclose(archivo);
}