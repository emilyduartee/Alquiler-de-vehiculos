#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reportes.h"
#include "clientes.h"
#include "vehiculos.h"
#include "alquileres.h"
#include "movimientos.h"
#include "comun.h"
#include "utils.h"
#include "fechas.h"

void reporteVehiculosDisponibles(void) {
    limpiarPantalla();
    printf("\n========================================================================================\n");
    printf("                       REPORTE 1: VEHICULOS DISPONIBLES\n");
    printf("========================================================================================\n");

    FILE *f = fopen(ARCH_VEHICULOS, "rb");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s o no hay vehiculos registrados.\n", ARCH_VEHICULOS);
        pausarPantalla();
        return;
    }

    printf("%-10s %-12s %-12s %-6s %-10s %-6s %-12s\n",
           "PATENTE", "MARCA", "MODELO", "ANIO", "TIPO", "CAP.", "PRECIO/DIA");
    printf("----------------------------------------------------------------------------------------\n");

    struct Vehiculo v;
    int cont = 0;
    while (fread(&v, sizeof(struct Vehiculo), 1, f) == 1) {
        if (strcmp(v.estado, EST_VEH_DISPONIBLE) == 0) {
            printf("%-10s %-12s %-12s %-6d %-10s %-6d $%10.2f\n",
                   v.patente, v.marca, v.modelo, v.anio, v.tipo, v.capacidad, v.precio_dia);
            cont++;
        }
    }
    fclose(f);

    printf("----------------------------------------------------------------------------------------\n");
    printf("Total de vehiculos actualmente disponibles: %d\n", cont);
    pausarPantalla();
}

void reporteVehiculosAlquilados(void) {
    limpiarPantalla();
    printf("\n===================================================================================================\n");
    printf("                               REPORTE 2: VEHICULOS ALQUILADOS\n");
    printf("===================================================================================================\n");

    FILE *f = fopen(ARCH_ALQUILERES, "rb");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s o no hay alquileres registrados.\n", ARCH_ALQUILERES);
        pausarPantalla();
        return;
    }

    printf("%-10s | %-22s | %-26s | %-12s | %-12s\n",
           "PATENTE", "MARCA Y MODELO", "CLIENTE (DNI Y NOMBRE)", "FECHA INICIO", "F. PREVISTA");
    printf("---------------------------------------------------------------------------------------------------\n");

    struct Alquiler a;
    int cont = 0;
    while (fread(&a, sizeof(struct Alquiler), 1, f) == 1) {
        if (strcmp(a.estado, EST_ALQ_ACTIVO) == 0) {
            struct Vehiculo v;
            char infoVeh[24];
            if (buscarVehiculoPorPatente(a.patente, &v)) {
                snprintf(infoVeh, sizeof(infoVeh), "%s %s", v.marca, v.modelo);
            } else {
                strcpy(infoVeh, "(Desconocido)");
            }

            struct Cliente c;
            char infoCli[28];
            if (buscarClientePorDNI(a.DNI, &c)) {
                snprintf(infoCli, sizeof(infoCli), "%d - %.14s", c.DNI, c.apellidoNombre);
            } else {
                snprintf(infoCli, sizeof(infoCli), "%d", a.DNI);
            }

            printf("%-10s | %-22s | %-26s | %-12d | %-12d\n",
                   a.patente, infoVeh, infoCli, a.fechaInicio, a.fechaPrevista);
            cont++;
        }
    }
    fclose(f);

    printf("---------------------------------------------------------------------------------------------------\n");
    printf("Total de vehiculos actualmente alquilados: %d\n", cont);
    pausarPantalla();
}

void reporteAlquileresPorRango(void) {
    limpiarPantalla();
    printf("\n=============================================================================================================================================\n");
    printf("                                              REPORTE 3: ALQUILERES POR RANGO DE FECHAS\n");
    printf("=============================================================================================================================================\n");

    int fIni = 0, fFin = 0;
    leerFecha("Ingrese la fecha inicial del rango (AAAAMMDD): ", &fIni);
    while (1) {
        leerFecha("Ingrese la fecha final del rango (AAAAMMDD): ", &fFin);
        if (fFin < fIni) {
            printf("[ERROR] La fecha final no puede ser anterior a la inicial (%d). Reintente.\n", fIni);
            continue;
        }
        break;
    }

    FILE *f = fopen(ARCH_ALQUILERES, "rb");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s.\n", ARCH_ALQUILERES);
        pausarPantalla();
        return;
    }

    printf("\nAlquileres cuya fecha de inicio se encuentra entre %d y %d:\n\n", fIni, fFin);
    imprimirEncabezadoAlquileres();

    struct Alquiler a;
    int cont = 0;
    while (fread(&a, sizeof(struct Alquiler), 1, f) == 1) {
        if (a.fechaInicio >= fIni && a.fechaInicio <= fFin) {
            imprimirFilaAlquiler(&a);
            cont++;
        }
    }
    fclose(f);

    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("Total de alquileres encontrados en el rango: %d\n", cont);
    pausarPantalla();
}

void reporteAlquileresPorCliente(void) {
    limpiarPantalla();
    printf("\n=============================================================================================================================================\n");
    printf("                                                 REPORTE 4: ALQUILERES POR CLIENTE\n");
    printf("=============================================================================================================================================\n");

    int dni = 0;
    leerDNI("Ingrese el DNI del cliente: ", &dni);

    struct Cliente c;
    if (!buscarClientePorDNI(dni, &c)) {
        printf("\n[ERROR] El cliente con DNI %d no existe en el sistema.\n", dni);
        pausarPantalla();
        return;
    }

    printf("\nCliente: %s (DNI: %d) | Telefono: %s | Estado: %s\n\n",
           c.apellidoNombre, c.DNI, c.telefono, (c.estado == CLI_ACTIVO) ? "ACTIVO" : "INACTIVO");

    FILE *f = fopen(ARCH_ALQUILERES, "rb");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s.\n", ARCH_ALQUILERES);
        pausarPantalla();
        return;
    }

    imprimirEncabezadoAlquileres();
    struct Alquiler a;
    int cont = 0;
    while (fread(&a, sizeof(struct Alquiler), 1, f) == 1) {
        if (a.DNI == dni) {
            imprimirFilaAlquiler(&a);
            cont++;
        }
    }
    fclose(f);

    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("Total de alquileres registrados para este cliente: %d\n", cont);
    pausarPantalla();
}

void reporteAlquileresPorVehiculo(void) {
    limpiarPantalla();
    printf("\n=============================================================================================================================================\n");
    printf("                                                REPORTE 5: ALQUILERES POR VEHICULO\n");
    printf("=============================================================================================================================================\n");

    char pat[LON_PATENTE];
    leerPatente("Ingrese la patente del vehiculo: ", pat);

    struct Vehiculo v;
    if (!buscarVehiculoPorPatente(pat, &v)) {
        printf("\n[ERROR] El vehiculo con patente %s no existe en el sistema.\n", pat);
        pausarPantalla();
        return;
    }

    printf("\nVehiculo: %s %s (Patente: %s) | Tipo: %s | Tarifa/dia: $%.2f | Estado: %s\n\n",
           v.marca, v.modelo, v.patente, v.tipo, v.precio_dia, v.estado);

    FILE *f = fopen(ARCH_ALQUILERES, "rb");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s.\n", ARCH_ALQUILERES);
        pausarPantalla();
        return;
    }

    imprimirEncabezadoAlquileres();
    struct Alquiler a;
    int cont = 0;
    while (fread(&a, sizeof(struct Alquiler), 1, f) == 1) {
        if (strcmp(a.patente, pat) == 0) {
            imprimirFilaAlquiler(&a);
            cont++;
        }
    }
    fclose(f);

    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("Total de alquileres historicos para este vehiculo: %d\n", cont);
    pausarPantalla();
}

void reporteAlquileresPorEstado(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("     REPORTE 6: ALQUILERES POR ESTADO\n");
    printf("========================================\n");
    printf("1. Activos\n");
    printf("2. Finalizados\n");
    printf("3. Cancelados\n");
    printf("0. Cancelar\n");

    int opc = 0;
    leerEntero("Seleccione un estado: ", 0, 3, &opc);
    if (opc == 0) return;

    const char *estadoBuscado = EST_ALQ_ACTIVO;
    if (opc == 2) estadoBuscado = EST_ALQ_FINALIZADO;
    else if (opc == 3) estadoBuscado = EST_ALQ_CANCELADO;

    FILE *f = fopen(ARCH_ALQUILERES, "rb");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s.\n", ARCH_ALQUILERES);
        pausarPantalla();
        return;
    }

    limpiarPantalla();
    printf("\n=============================================================================================================================================\n");
    printf("                                              LISTADO DE ALQUILERES EN ESTADO: %s\n", estadoBuscado);
    printf("=============================================================================================================================================\n");

    imprimirEncabezadoAlquileres();
    struct Alquiler a;
    int cont = 0;
    while (fread(&a, sizeof(struct Alquiler), 1, f) == 1) {
        if (strcmp(a.estado, estadoBuscado) == 0) {
            imprimirFilaAlquiler(&a);
            cont++;
        }
    }
    fclose(f);

    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("Total de alquileres en estado %s: %d\n", estadoBuscado, cont);
    pausarPantalla();
}

void reporteAlquileresPorImporte(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("    REPORTE 7: ALQUILERES POR IMPORTE\n");
    printf("========================================\n");

    double montoRef = 0.0;
    leerDouble("Ingrese el importe de referencia: ", 0.0, &montoRef);

    printf("\nSeleccione el criterio de comparacion:\n");
    printf("1. Superiores al importe ingresado (> $%.2f)\n", montoRef);
    printf("2. Inferiores al importe ingresado (< $%.2f)\n", montoRef);
    int criterio = 1;
    leerEntero("Opcion: ", 1, 2, &criterio);

    FILE *f = fopen(ARCH_ALQUILERES, "rb");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s.\n", ARCH_ALQUILERES);
        pausarPantalla();
        return;
    }

    limpiarPantalla();
    printf("\n=============================================================================================================================================\n");
    if (criterio == 1) {
        printf("                                    ALQUILERES CON IMPORTE TOTAL SUPERIOR A $%.2f\n", montoRef);
    } else {
        printf("                                    ALQUILERES CON IMPORTE TOTAL INFERIOR A $%.2f\n", montoRef);
    }
    printf("=============================================================================================================================================\n");

    imprimirEncabezadoAlquileres();
    struct Alquiler a;
    int cont = 0;
    while (fread(&a, sizeof(struct Alquiler), 1, f) == 1) {
        int coincide = 0;
        if (criterio == 1 && a.importeTotal > montoRef) {
            coincide = 1;
        } else if (criterio == 2 && a.importeTotal < montoRef) {
            coincide = 1;
        }
        if (coincide) {
            imprimirFilaAlquiler(&a);
            cont++;
        }
    }
    fclose(f);

    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("Total de alquileres que cumplen la condicion: %d\n", cont);
    pausarPantalla();
}

void reportePagosPorPeriodo(void) {
    limpiarPantalla();
    printf("\n========================================================================================\n");
    printf("                       REPORTE 8: PAGOS REALIZADOS POR PERIODO\n");
    printf("========================================================================================\n");

    int fIni = 0, fFin = 0;
    leerFecha("Ingrese la fecha inicial del periodo (AAAAMMDD): ", &fIni);
    while (1) {
        leerFecha("Ingrese la fecha final del periodo (AAAAMMDD): ", &fFin);
        if (fFin < fIni) {
            printf("[ERROR] La fecha final no puede ser anterior a la inicial (%d). Reintente.\n", fIni);
            continue;
        }
        break;
    }

    FILE *f = fopen(ARCH_MOVIMIENTOS, "rb");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s o no hay movimientos registrados.\n", ARCH_MOVIMIENTOS);
        pausarPantalla();
        return;
    }

    printf("\nPagos registrados entre %d y %d:\n\n", fIni, fFin);
    printf("%-6s | %-6s | %-10s | %-10s | %-12s | %-10s | %-14s\n",
           "MOV", "ALQ", "DNI", "FECHA", "MONTO", "PATENTE", "MEDIO");
    printf("----------------------------------------------------------------------------------------\n");

    struct Movimiento m;
    int cont = 0;
    double totalPeriodo = 0.0;
    while (fread(&m, sizeof(struct Movimiento), 1, f) == 1) {
        if (strcmp(m.tipoMov, MOV_PAGO) == 0 && m.fecha >= fIni && m.fecha <= fFin) {
            printf("%-6d | %-6d | %-10d | %-10d | $%11.2f | %-10s | %-14s\n",
                   m.numMovimiento, m.numAlquiler, m.DNI, m.fecha, m.monto, m.patente, m.medioPago);
            totalPeriodo += m.monto;
            cont++;
        }
    }
    fclose(f);

    printf("----------------------------------------------------------------------------------------\n");
    printf("Cantidad de pagos en el periodo: %d | Monto total acumulado: $%.2f\n", cont, totalPeriodo);
    pausarPantalla();
}

void reporteClientesConDeuda(void) {
    limpiarPantalla();
    printf("\n========================================================================================================\n");
    printf("                                   REPORTE 9: CLIENTES CON DEUDA\n");
    printf("========================================================================================================\n");

    FILE *f = fopen(ARCH_ALQUILERES, "rb");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s.\n", ARCH_ALQUILERES);
        pausarPantalla();
        return;
    }

    printf("%-10s | %-24s | %-5s | %-12s | %-12s | %-12s | %-10s\n",
           "DNI", "APELLIDO Y NOMBRE", "ALQ", "TOTAL", "PAGADO", "SALDO DEUDA", "ESTADO");
    printf("--------------------------------------------------------------------------------------------------------\n");

    struct Alquiler a;
    int cont = 0;
    double totalDeudaGeneral = 0.0;
    while (fread(&a, sizeof(struct Alquiler), 1, f) == 1) {
        if (strcmp(a.estado, EST_ALQ_CANCELADO) != 0) {
            double saldo = a.importeTotal - a.importePagado;
            if (saldo > TOLERANCIA_SALDO) {
                struct Cliente c;
                char nombreCli[30];
                if (buscarClientePorDNI(a.DNI, &c)) {
                    snprintf(nombreCli, sizeof(nombreCli), "%s", c.apellidoNombre);
                } else {
                    strcpy(nombreCli, "(Desconocido)");
                }
                printf("%-10d | %-24.24s | %-5d | $%11.2f | $%11.2f | $%11.2f | %-10s\n",
                       a.DNI, nombreCli, a.numAlquiler, a.importeTotal, a.importePagado, saldo, a.estado);
                totalDeudaGeneral += saldo;
                cont++;
            }
        }
    }
    fclose(f);

    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("Total de alquileres con saldo pendiente: %d\n", cont);
    printf("DEUDA TOTAL GENERAL ACUMULADA: $%.2f\n", totalDeudaGeneral);
    pausarPantalla();
}

void reporteEstadisticas(void) {
    limpiarPantalla();
    printf("\n========================================================================================\n");
    printf("                           REPORTE 10: ESTADISTICAS DE LA EMPRESA\n");
    printf("========================================================================================\n");

    /* Flota de vehiculos */
    int totalVeh = 0, vehDisponibles = 0, vehAlquilados = 0;
    int vehMantenimiento = 0, vehBaja = 0;
    FILE *fVeh = fopen(ARCH_VEHICULOS, "rb");
    if (fVeh) {
        struct Vehiculo v;
        while (fread(&v, sizeof(struct Vehiculo), 1, fVeh) == 1) {
            totalVeh++;
            if (strcmp(v.estado, EST_VEH_DISPONIBLE) == 0) vehDisponibles++;
            else if (strcmp(v.estado, EST_VEH_ALQUILADO) == 0) vehAlquilados++;
            else if (strcmp(v.estado, EST_VEH_MANT) == 0) vehMantenimiento++;
            else if (strcmp(v.estado, EST_VEH_BAJA) == 0) vehBaja++;
        }
        fclose(fVeh);
    }

    /* Alquileres y finanzas */
    int totalAlq = 0, alqActivos = 0, alqFinalizados = 0, alqCancelados = 0;
    double cobradoTotal = 0.0;
    double pendienteTotal = 0.0;
    FILE *fAlq = fopen(ARCH_ALQUILERES, "rb");
    if (fAlq) {
        struct Alquiler a;
        while (fread(&a, sizeof(struct Alquiler), 1, fAlq) == 1) {
            totalAlq++;
            cobradoTotal += a.importePagado;
            if (strcmp(a.estado, EST_ALQ_ACTIVO) == 0) {
                alqActivos++;
            } else if (strcmp(a.estado, EST_ALQ_FINALIZADO) == 0) {
                alqFinalizados++;
            } else if (strcmp(a.estado, EST_ALQ_CANCELADO) == 0) {
                alqCancelados++;
            }
            if (strcmp(a.estado, EST_ALQ_CANCELADO) != 0) {
                double saldo = a.importeTotal - a.importePagado;
                if (saldo > TOLERANCIA_SALDO) {
                    pendienteTotal += saldo;
                }
            }
        }
        fclose(fAlq);
    }

    printf("\n--- PARQUE AUTOMOTOR ---\n");
    printf("Total de vehiculos:             %d\n", totalVeh);
    printf("  Vehiculos disponibles:        %d\n", vehDisponibles);
    printf("  Vehiculos alquilados:         %d\n", vehAlquilados);
    printf("  Vehiculos en mantenimiento:   %d\n", vehMantenimiento);
    printf("  Vehiculos dados de baja:      %d\n", vehBaja);

    printf("\n--- OPERACIONES DE ALQUILER ---\n");
    printf("Total de alquileres historicos: %d\n", totalAlq);
    printf("  Alquileres activos:           %d\n", alqActivos);
    printf("  Alquileres finalizados:       %d\n", alqFinalizados);
    printf("  Alquileres cancelados:        %d\n", alqCancelados);

    printf("\n--- SITUACION ECONOMICA Y FINANCIERA ---\n");
    printf("Importe Total Cobrado:          $%.2f\n", cobradoTotal);
    printf("Importe Pendiente de Cobro:     $%.2f\n", pendienteTotal);
    printf("========================================================================================\n");
    pausarPantalla();
}

void submenuReportes(void) {
    int opc = -1;
    do {
        limpiarPantalla();
        printf("\n========================================\n");
        printf("     SUBMENU CONSULTAS Y REPORTES\n");
        printf("========================================\n");
        printf(" 1. Vehiculos disponibles\n");
        printf(" 2. Vehiculos alquilados\n");
        printf(" 3. Alquileres por rango de fechas\n");
        printf(" 4. Alquileres por cliente\n");
        printf(" 5. Alquileres por vehiculo\n");
        printf(" 6. Alquileres por estado\n");
        printf(" 7. Alquileres por importe\n");
        printf(" 8. Pagos realizados por periodo\n");
        printf(" 9. Clientes con deuda\n");
        printf("10. Estadisticas de la empresa\n");
        printf(" 0. Volver al Menu Principal\n");
        printf("========================================\n");
        leerEntero("Seleccione una opcion (0-10): ", 0, 10, &opc);

        switch (opc) {
            case 1: reporteVehiculosDisponibles(); break;
            case 2: reporteVehiculosAlquilados(); break;
            case 3: reporteAlquileresPorRango(); break;
            case 4: reporteAlquileresPorCliente(); break;
            case 5: reporteAlquileresPorVehiculo(); break;
            case 6: reporteAlquileresPorEstado(); break;
            case 7: reporteAlquileresPorImporte(); break;
            case 8: reportePagosPorPeriodo(); break;
            case 9: reporteClientesConDeuda(); break;
            case 10: reporteEstadisticas(); break;
            case 0: break;
            default:
                printf("\n[ERROR] Opcion no valida.\n");
                pausarPantalla();
                break;
        }
    } while (opc != 0);
}
