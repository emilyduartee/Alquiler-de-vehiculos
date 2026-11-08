#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alquileres.h"
#include "clientes.h"
#include "vehiculos.h"
#include "movimientos.h"
#include "comun.h"
#include "utils.h"
#include "fechas.h"

int buscarAlquilerPorNumero(int num, struct Alquiler *out) {
    FILE *f = fopen(ARCH_ALQUILERES, "rb");
    if (!f) {
        return 0;
    }
    struct Alquiler a;
    int encontrado = 0;
    while (fread(&a, sizeof(struct Alquiler), 1, f) == 1) {
        if (a.numAlquiler == num) {
            if (out) {
                *out = a;
            }
            encontrado = 1;
            break;
        }
    }
    fclose(f);
    return encontrado;
}

int buscarAlquilerActivoPorPatente(const char *pat, struct Alquiler *out) {
    FILE *f = fopen(ARCH_ALQUILERES, "rb");
    if (!f) {
        return 0;
    }
    struct Alquiler a;
    int encontrado = 0;
    while (fread(&a, sizeof(struct Alquiler), 1, f) == 1) {
        if (strcmp(a.patente, pat) == 0 && strcmp(a.estado, EST_ALQ_ACTIVO) == 0) {
            if (out) {
                *out = a;
            }
            encontrado = 1;
            break;
        }
    }
    fclose(f);
    return encontrado;
}

int clienteTieneAlquilerActivo(int dni) {
    FILE *f = fopen(ARCH_ALQUILERES, "rb");
    if (!f) {
        return 0;
    }
    struct Alquiler a;
    int tiene = 0;
    while (fread(&a, sizeof(struct Alquiler), 1, f) == 1) {
        if (a.DNI == dni && strcmp(a.estado, EST_ALQ_ACTIVO) == 0) {
            tiene = a.numAlquiler;
            break;
        }
    }
    fclose(f);
    return tiene;
}

int vehiculoTieneAlquilerActivo(const char *pat) {
    struct Alquiler a;
    return buscarAlquilerActivoPorPatente(pat, &a);
}

int guardarAlquiler(const struct Alquiler *a) {
    FILE *f = fopen(ARCH_ALQUILERES, "ab");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s para escritura.\n", ARCH_ALQUILERES);
        return 0;
    }
    size_t escritos = fwrite(a, sizeof(struct Alquiler), 1, f);
    fclose(f);
    return escritos == 1;
}

int actualizarAlquiler(const struct Alquiler *a) {
    FILE *f = fopen(ARCH_ALQUILERES, "rb+");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s para actualizacion.\n", ARCH_ALQUILERES);
        return 0;
    }
    struct Alquiler leido;
    int actualizado = 0;
    while (fread(&leido, sizeof(struct Alquiler), 1, f) == 1) {
        if (leido.numAlquiler == a->numAlquiler) {
            if (fseek(f, -(long)sizeof(struct Alquiler), SEEK_CUR) != 0) {
                break;
            }
            if (fwrite(a, sizeof(struct Alquiler), 1, f) == 1) {
                actualizado = 1;
            }
            break;
        }
    }
    fclose(f);
    return actualizado;
}

int proximoNumeroAlquiler(void) {
    FILE *f = fopen(ARCH_ALQUILERES, "rb");
    if (!f) {
        return 1;
    }
    struct Alquiler a;
    int maxNum = 0;
    while (fread(&a, sizeof(struct Alquiler), 1, f) == 1) {
        if (a.numAlquiler > maxNum) {
            maxNum = a.numAlquiler;
        }
    }
    fclose(f);
    return maxNum + 1;
}

void imprimirEncabezadoAlquileres(void) {
    printf("%-5s | %-24s | %-24s | %-8s | %-8s | %-8s | %-4s | %-11s | %-11s | %-11s\n",
           "NRO", "CLIENTE (DNI - NOMBRE)", "VEHICULO (PAT - MARCA)", "INICIO", "PREVISTA", "DEV.", "DIAS", "TOTAL", "PAGADO", "ESTADO");
    printf("---------------------------------------------------------------------------------------------------------------------------------------------\n");
}

void imprimirFilaAlquiler(const struct Alquiler *a) {
    struct Cliente c;
    char infoCliente[32];
    if (buscarClientePorDNI(a->DNI, &c)) {
        snprintf(infoCliente, sizeof(infoCliente), "%d - %.12s", c.DNI, c.apellidoNombre);
    } else {
        snprintf(infoCliente, sizeof(infoCliente), "%d - (Desc)", a->DNI);
    }

    struct Vehiculo v;
    char infoVehiculo[32];
    if (buscarVehiculoPorPatente(a->patente, &v)) {
        snprintf(infoVehiculo, sizeof(infoVehiculo), "%s - %.10s", v.patente, v.marca);
    } else {
        snprintf(infoVehiculo, sizeof(infoVehiculo), "%s - (Desc)", a->patente);
    }

    char fDevStr[16];
    if (a->fechaEfectiva > 0) {
        snprintf(fDevStr, sizeof(fDevStr), "%d", a->fechaEfectiva);
    } else {
        strcpy(fDevStr, "-");
    }

    printf("%-5d | %-24s | %-24s | %-8d | %-8d | %-8s | %-4d | $%10.2f | $%10.2f | %-11s\n",
           a->numAlquiler, infoCliente, infoVehiculo, a->fechaInicio, a->fechaPrevista,
           fDevStr, a->cantDias, a->importeTotal, a->importePagado, a->estado);
}

void realizarAlquiler(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("           NUEVO ALQUILER\n");
    printf("========================================\n");

    int dni = 0;
    leerDNI("Ingrese el DNI del cliente: ", &dni);

    struct Cliente c;
    if (!buscarClientePorDNI(dni, &c)) {
        printf("\n[ERROR] El cliente con DNI %d no existe en el sistema.\n", dni);
        pausarPantalla();
        return;
    }
    if (c.estado != CLI_ACTIVO) {
        printf("\n[ERROR] El cliente se encuentra INACTIVO. Operacion rechazada.\n");
        pausarPantalla();
        return;
    }

    int alqActivoExistente = clienteTieneAlquilerActivo(dni);
    if (alqActivoExistente > 0) {
        printf("\n[ERROR] El cliente ya posee el alquiler Nro %d en estado ACTIVO.\n", alqActivoExistente);
        printf("Cada cliente puede tener como maximo un alquiler activo a la vez.\n");
        pausarPantalla();
        return;
    }

    char pat[LON_PATENTE];
    leerPatente("Ingrese la patente del vehiculo a alquilar: ", pat);

    struct Vehiculo v;
    if (!buscarVehiculoPorPatente(pat, &v)) {
        printf("\n[ERROR] El vehiculo con patente %s no existe en el sistema.\n", pat);
        pausarPantalla();
        return;
    }
    if (strcmp(v.estado, EST_VEH_DISPONIBLE) != 0) {
        printf("\n[ERROR] El vehiculo no esta disponible para alquiler (Estado actual: %s).\n", v.estado);
        pausarPantalla();
        return;
    }

    int fechaIni = 0;
    leerFecha("Ingrese la fecha de inicio del alquiler (AAAAMMDD): ", &fechaIni);

    int cantDias = 0;
    leerEntero("Ingrese la cantidad de dias contratados (> 0): ", 1, 365, &cantDias);

    struct Alquiler nuevo;
    memset(&nuevo, 0, sizeof(struct Alquiler));

    nuevo.numAlquiler = proximoNumeroAlquiler();
    nuevo.DNI = dni;
    strcpy(nuevo.patente, pat);
    nuevo.fechaInicio = fechaIni;
    nuevo.cantDias = cantDias;

    /* B2: Calculo correcto de fecha prevista con sumarDias */
    nuevo.fechaPrevista = sumarDias(fechaIni, cantDias);
    nuevo.fechaEfectiva = 0;
    nuevo.importeTotal = cantDias * v.precio_dia;
    nuevo.importePagado = 0.0;
    strcpy(nuevo.estado, EST_ALQ_ACTIVO);

    /* Guardar alquiler y actualizar estado de vehiculo a Alquilado */
    if (!guardarAlquiler(&nuevo)) {
        printf("\n[ERROR] No se pudo guardar el alquiler en %s.\n", ARCH_ALQUILERES);
        pausarPantalla();
        return;
    }
    actualizarEstadoVehiculo(pat, EST_VEH_ALQUILADO);

    printf("\n========================================\n");
    printf(">>> ALQUILER REGISTRADO EXITOSAMENTE!\n");
    printf("Numero de Alquiler: %d\n", nuevo.numAlquiler);
    printf("Cliente:            %s (DNI: %d)\n", c.apellidoNombre, c.DNI);
    printf("Vehiculo:           %s %s (Patente: %s)\n", v.marca, v.modelo, v.patente);
    printf("Fecha de Inicio:    %d\n", nuevo.fechaInicio);
    printf("Fecha Prevista:     %d\n", nuevo.fechaPrevista);
    printf("Dias Contratados:   %d\n", nuevo.cantDias);
    printf("Importe Total:      $%.2f\n", nuevo.importeTotal);
    printf("Estado Alquiler:    %s\n", nuevo.estado);
    printf("Estado Vehiculo:    ALQUILADO\n");
    printf("========================================\n");
    pausarPantalla();
}

void registrarDevolucion(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("        REGISTRAR DEVOLUCION\n");
    printf("========================================\n");
    printf("Seleccione metodo de identificacion:\n");
    printf("1. Por Patente de Vehiculo\n");
    printf("2. Por Numero de Alquiler\n");
    printf("0. Cancelar\n");

    int metodo = 0;
    leerEntero("Opcion: ", 0, 2, &metodo);
    if (metodo == 0) return;

    struct Alquiler a;
    int encontrado = 0;

    if (metodo == 1) {
        char pat[LON_PATENTE];
        leerPatente("Ingrese la patente del vehiculo devuelto: ", pat);
        if (buscarAlquilerActivoPorPatente(pat, &a)) {
            encontrado = 1;
        } else {
            printf("\n[ERROR] No existe ningun alquiler ACTIVO para el vehiculo con patente %s.\n", pat);
        }
    } else {
        int numAlq = 0;
        leerEntero("Ingrese el numero de alquiler: ", 1, 9999999, &numAlq);
        if (buscarAlquilerPorNumero(numAlq, &a)) {
            if (strcmp(a.estado, EST_ALQ_ACTIVO) == 0) {
                encontrado = 1;
            } else {
                printf("\n[ERROR] El alquiler Nro %d no se encuentra en estado ACTIVO (Estado: %s).\n",
                       numAlq, a.estado);
            }
        } else {
            printf("\n[ERROR] El alquiler Nro %d no existe.\n", numAlq);
        }
    }

    if (!encontrado) {
        pausarPantalla();
        return;
    }

    struct Vehiculo v;
    if (!buscarVehiculoPorPatente(a.patente, &v)) {
        printf("\n[ERROR] Datos de vehiculo asociados a la patente %s no encontrados.\n", a.patente);
        pausarPantalla();
        return;
    }

    printf("\nAlquiler ACTIVO localizado: Nro %d\n", a.numAlquiler);
    printf("Vehiculo: %s %s (Patente: %s) | Tarifa/dia: $%.2f\n",
           v.marca, v.modelo, v.patente, v.precio_dia);
    printf("Fecha Inicio: %d | Fecha Prevista: %d\n", a.fechaInicio, a.fechaPrevista);
    printf("----------------------------------------\n");

    int fDev = 0;
    while (1) {
        leerFecha("Ingrese la fecha efectiva de devolucion (AAAAMMDD): ", &fDev);
        if (fDev < a.fechaInicio) {
            printf("[ERROR] La fecha de devolucion no puede ser anterior a la fecha de inicio (%d). Reintente.\n",
                   a.fechaInicio);
            continue;
        }
        break;
    }

    /* B3: Calculo exacto de dias adicionales con calendario civil */
    int diasAdic = diasEntre(a.fechaPrevista, fDev);
    double recargo = 0.0;
    if (diasAdic > 0) {
        recargo = diasAdic * v.precio_dia;
        a.importeTotal += recargo;
        printf("\n[AVISO DE EXCEDENTE] Se detectaron %d dia(s) de demora.\n", diasAdic);
        printf("Recargo automatico aplicado: $%.2f (Tarifa/dia: $%.2f)\n", recargo, v.precio_dia);

        /* A14: Registrar Movimiento tipo Ajuste */
        registrarMovimiento(a.numAlquiler, MOV_AJUSTE, recargo, fDev, MEDIO_NINGUNO);
    } else {
        printf("\nVehiculo devuelto dentro del plazo acordado o de forma anticipada. Sin recargo.\n");
    }

    a.fechaEfectiva = fDev;
    strcpy(a.estado, EST_ALQ_FINALIZADO);

    if (!actualizarAlquiler(&a)) {
        printf("\n[ERROR] No se pudo actualizar el registro del alquiler.\n");
        pausarPantalla();
        return;
    }
    actualizarEstadoVehiculo(a.patente, EST_VEH_DISPONIBLE);

    printf("\n========================================\n");
    printf(">>> DEVOLUCION REGISTRADA EXITOSAMENTE!\n");
    printf("Alquiler Nro:            %d\n", a.numAlquiler);
    printf("Fecha Efectiva:          %d\n", a.fechaEfectiva);
    printf("Importe Total Final:     $%.2f\n", a.importeTotal);
    printf("Importe Pagado:          $%.2f\n", a.importePagado);
    printf("Saldo Pendiente de Pago: $%.2f\n", a.importeTotal - a.importePagado);
    printf("Estado del Vehiculo:     DISPONIBLE\n");
    printf("========================================\n");
    pausarPantalla();
}

void cancelarAlquiler(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("          CANCELAR ALQUILER\n");
    printf("========================================\n");

    int numAlq = 0;
    leerEntero("Ingrese el numero de alquiler a cancelar: ", 1, 9999999, &numAlq);

    struct Alquiler a;
    if (!buscarAlquilerPorNumero(numAlq, &a)) {
        printf("\n[ERROR] El alquiler Nro %d no existe.\n", numAlq);
        pausarPantalla();
        return;
    }

    if (strcmp(a.estado, EST_ALQ_ACTIVO) != 0) {
        printf("\n[ERROR] Solo se pueden cancelar alquileres en estado ACTIVO (Estado actual: %s).\n",
               a.estado);
        pausarPantalla();
        return;
    }

    printf("\nAlquiler Nro: %d | Cliente DNI: %d | Patente: %s\n", a.numAlquiler, a.DNI, a.patente);
    printf("Importe Total: $%.2f | Importe Pagado: $%.2f\n", a.importeTotal, a.importePagado);
    printf("----------------------------------------\n");

    if (!leerSN("Desea confirmar la cancelacion del alquiler (S/N)?: ")) {
        printf("\nOperacion cancelada por el usuario.\n");
        pausarPantalla();
        return;
    }

    double totalAnterior = a.importeTotal;
    /* Regla de negocio seccion 5.3: importeTotal = importePagado (saldo 0) */
    a.importeTotal = a.importePagado;
    strcpy(a.estado, EST_ALQ_CANCELADO);

    if (!actualizarAlquiler(&a)) {
        printf("\n[ERROR] No se pudo actualizar el estado del alquiler a Cancelado.\n");
        pausarPantalla();
        return;
    }
    actualizarEstadoVehiculo(a.patente, EST_VEH_DISPONIBLE);

    /* A14: Si cambio el total, registrar movimiento de Ajuste negativo */
    if (a.importePagado < totalAnterior) {
        double ajusteNegativo = a.importePagado - totalAnterior;
        registrarMovimiento(a.numAlquiler, MOV_AJUSTE, ajusteNegativo, fechaActual(), MEDIO_NINGUNO);
    }

    printf("\n========================================\n");
    printf(">>> ALQUILER CANCELADO EXITOSAMENTE!\n");
    printf("Estado del Alquiler: CANCELADO\n");
    printf("Importe Retenido:    $%.2f\n", a.importePagado);
    printf("Saldo Pendiente:     $0.00\n");
    printf("Estado del Vehiculo: DISPONIBLE\n");
    printf("========================================\n");
    pausarPantalla();
}

void modificarAlquiler(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("          MODIFICAR ALQUILER\n");
    printf("========================================\n");

    int numAlq = 0;
    leerEntero("Ingrese el numero de alquiler a modificar: ", 1, 9999999, &numAlq);

    struct Alquiler a;
    if (!buscarAlquilerPorNumero(numAlq, &a)) {
        printf("\n[ERROR] El alquiler Nro %d no existe.\n", numAlq);
        pausarPantalla();
        return;
    }
    if (strcmp(a.estado, EST_ALQ_ACTIVO) != 0) {
        printf("\n[ERROR] Solo se pueden modificar alquileres en estado ACTIVO (Estado actual: %s).\n",
               a.estado);
        pausarPantalla();
        return;
    }

    struct Vehiculo vActual;
    buscarVehiculoPorPatente(a.patente, &vActual);

    printf("\nAlquiler Nro: %d | Vehiculo: %s (%s) | Tarifa/dia: $%.2f\n",
           a.numAlquiler, a.patente, vActual.modelo, vActual.precio_dia);
    printf("Inicio: %d | Dias: %d | Prevista: %d | Total: $%.2f | Pagado: $%.2f\n",
           a.fechaInicio, a.cantDias, a.fechaPrevista, a.importeTotal, a.importePagado);
    printf("--------------------------------------------------------------------\n");

    /* Preguntar si cambia vehiculo */
    if (leerSN("Desea cambiar el vehiculo asignado (S/N)?: ")) {
        char nuevaPat[LON_PATENTE];
        leerPatente("Ingrese la nueva patente: ", nuevaPat);
        struct Vehiculo vNueva;
        if (!buscarVehiculoPorPatente(nuevaPat, &vNueva)) {
            printf("[ERROR] El nuevo vehiculo no existe.\n");
        } else if (strcmp(vNueva.estado, EST_VEH_DISPONIBLE) != 0) {
            printf("[ERROR] El nuevo vehiculo no esta disponible (Estado: %s).\n", vNueva.estado);
        } else {
            actualizarEstadoVehiculo(a.patente, EST_VEH_DISPONIBLE);
            strcpy(a.patente, nuevaPat);
            actualizarEstadoVehiculo(nuevaPat, EST_VEH_ALQUILADO);
            vActual = vNueva;
            printf("Vehiculo cambiado exitosamente a %s (%s %s).\n",
                   vActual.patente, vActual.marca, vActual.modelo);
        }
    }

    if (leerSN("Desea cambiar la fecha de inicio (S/N)?: ")) {
        leerFecha("Ingrese nueva fecha de inicio (AAAAMMDD): ", &a.fechaInicio);
    }

    if (leerSN("Desea cambiar la cantidad de dias contratados (S/N)?: ")) {
        leerEntero("Ingrese nueva cantidad de dias (> 0): ", 1, 365, &a.cantDias);
    }

    /* B2: Recalcular fecha prevista */
    a.fechaPrevista = sumarDias(a.fechaInicio, a.cantDias);

    /* B7: Recalcular importe total y validar consistencia con importe pagado */
    double nuevoTotal = a.cantDias * vActual.precio_dia;
    if (nuevoTotal < a.importePagado) {
        printf("\n[ERROR] El nuevo importe total ($%.2f) no puede ser menor a lo ya pagado ($%.2f).\n",
               nuevoTotal, a.importePagado);
        printf("Modificacion cancelada.\n");
        pausarPantalla();
        return;
    }

    double difTotal = nuevoTotal - a.importeTotal;
    a.importeTotal = nuevoTotal;

    if (!actualizarAlquiler(&a)) {
        printf("\n[ERROR] No se pudo guardar la modificacion del alquiler.\n");
        pausarPantalla();
        return;
    }

    /* A14: Registrar Ajuste si el importe total cambio */
    if (difTotal > TOLERANCIA_SALDO || difTotal < -TOLERANCIA_SALDO) {
        registrarMovimiento(a.numAlquiler, MOV_AJUSTE, difTotal, fechaActual(), MEDIO_NINGUNO);
    }

    printf("\n>>> Alquiler modificado exitosamente!\n");
    printf("Nueva Fecha Prevista: %d\n", a.fechaPrevista);
    printf("Nuevo Importe Total:  $%.2f\n", a.importeTotal);
    pausarPantalla();
}

void listarAlquileres(void) {
    limpiarPantalla();
    printf("\n=============================================================================================================================================\n");
    printf("                                                     LISTADO GENERAL DE ALQUILERES\n");
    printf("=============================================================================================================================================\n");

    FILE *f = fopen(ARCH_ALQUILERES, "rb");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s o no hay alquileres registrados.\n", ARCH_ALQUILERES);
        pausarPantalla();
        return;
    }

    imprimirEncabezadoAlquileres();
    struct Alquiler a;
    int cont = 0;
    while (fread(&a, sizeof(struct Alquiler), 1, f) == 1) {
        imprimirFilaAlquiler(&a);
        cont++;
    }
    fclose(f);

    if (cont == 0) {
        printf("No hay alquileres registrados.\n");
    } else {
        printf("\nTotal de alquileres registrados: %d\n", cont);
    }
    pausarPantalla();
}

void submenuAlquileres(void) {
    int opc = -1;
    do {
        limpiarPantalla();
        printf("\n========================================\n");
        printf("           SUBMENU ALQUILERES\n");
        printf("========================================\n");
        printf("1. Realizar nuevo alquiler\n");
        printf("2. Modificar alquiler\n");
        printf("3. Listar alquileres\n");
        printf("4. Cancelar alquiler\n");
        printf("0. Volver al Menu Principal\n");
        printf("========================================\n");
        leerEntero("Seleccione una opcion: ", 0, 4, &opc);

        switch (opc) {
            case 1:
                realizarAlquiler();
                break;
            case 2:
                modificarAlquiler();
                break;
            case 3:
                listarAlquileres();
                break;
            case 4:
                cancelarAlquiler();
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
