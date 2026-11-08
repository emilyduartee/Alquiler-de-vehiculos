#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vehiculos.h"
#include "comun.h"
#include "utils.h"
#include "fechas.h"
#include "alquileres.h"

int buscarVehiculoPorPatente(const char *pat, struct Vehiculo *out) {
    FILE *f = fopen(ARCH_VEHICULOS, "rb");
    if (!f) {
        return 0;
    }
    struct Vehiculo v;
    int encontrado = 0;
    while (fread(&v, sizeof(struct Vehiculo), 1, f) == 1) {
        if (strcmp(v.patente, pat) == 0) {
            if (out) {
                *out = v;
            }
            encontrado = 1;
            break;
        }
    }
    fclose(f);
    return encontrado;
}

int existeVehiculo(const char *pat) {
    return buscarVehiculoPorPatente(pat, NULL);
}

int guardarVehiculo(const struct Vehiculo *v) {
    FILE *f = fopen(ARCH_VEHICULOS, "ab");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s para escritura.\n", ARCH_VEHICULOS);
        return 0;
    }
    size_t escritos = fwrite(v, sizeof(struct Vehiculo), 1, f);
    fclose(f);
    return escritos == 1;
}

int actualizarVehiculo(const struct Vehiculo *v) {
    FILE *f = fopen(ARCH_VEHICULOS, "rb+");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s para actualizacion.\n", ARCH_VEHICULOS);
        return 0;
    }
    struct Vehiculo leido;
    int actualizado = 0;
    while (fread(&leido, sizeof(struct Vehiculo), 1, f) == 1) {
        if (strcmp(leido.patente, v->patente) == 0) {
            if (fseek(f, -(long)sizeof(struct Vehiculo), SEEK_CUR) != 0) {
                break;
            }
            if (fwrite(v, sizeof(struct Vehiculo), 1, f) == 1) {
                actualizado = 1;
            }
            break;
        }
    }
    fclose(f);
    return actualizado;
}

int actualizarEstadoVehiculo(const char *pat, const char *estado) {
    struct Vehiculo v;
    if (!buscarVehiculoPorPatente(pat, &v)) {
        return 0;
    }
    strncpy(v.estado, estado, LON_ESTADO - 1);
    v.estado[LON_ESTADO - 1] = '\0';
    return actualizarVehiculo(&v);
}

void registrarVehiculo(void) {
    struct Vehiculo nuevo;
    memset(&nuevo, 0, sizeof(struct Vehiculo));

    limpiarPantalla();
    printf("\n========================================\n");
    printf("           ALTA DE VEHICULO\n");
    printf("========================================\n");

    leerPatente("Ingrese la patente: ", nuevo.patente);

    /* Validacion de patente unica */
    if (existeVehiculo(nuevo.patente)) {
        printf("\n[ERROR] La patente %s ya se encuentra registrada.\n", nuevo.patente);
        printf("Operacion cancelada.\n");
        pausarPantalla();
        return;
    }

    leerCadena("Ingrese la marca: ", nuevo.marca, LON_MARCA, 0);
    leerCadena("Ingrese el modelo: ", nuevo.modelo, LON_MODELO, 0);

    int anioMax = (fechaActual() / 10000) + 1;
    leerEntero("Ingrese el anio de fabricacion: ", 1900, anioMax, &nuevo.anio);

    leerCadena("Ingrese el tipo (ej. Auto, Camioneta, Utilitario): ", nuevo.tipo, LON_TIPO_VEH, 0);
    leerEntero("Ingrese la capacidad de personas: ", 1, 100, &nuevo.capacidad);
    leerDouble("Ingrese el precio de alquiler por dia: ", 0.0, &nuevo.precio_dia);

    strcpy(nuevo.estado, EST_VEH_DISPONIBLE);

    if (guardarVehiculo(&nuevo)) {
        printf("\n>>> Vehiculo registrado exitosamente (Estado: DISPONIBLE).\n");
    } else {
        printf("\n[ERROR] No se pudo guardar el vehiculo.\n");
    }
    pausarPantalla();
}

void modificarVehiculo(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("         MODIFICAR VEHICULO\n");
    printf("========================================\n");

    char pat[LON_PATENTE];
    leerPatente("Ingrese la patente del vehiculo a modificar: ", pat);

    struct Vehiculo v;
    if (!buscarVehiculoPorPatente(pat, &v)) {
        printf("\n[ERROR] Vehiculo con patente %s no encontrado.\n", pat);
        pausarPantalla();
        return;
    }

    printf("\nVehiculo encontrado: %s %s (Anio: %d, Estado: %s)\n",
           v.marca, v.modelo, v.anio, v.estado);
    printf("Precio por dia actual: $%.2f\n", v.precio_dia);
    printf("----------------------------------------\n");

    /* Validar si el vehiculo tiene un alquiler activo (B8) */
    int tieneAlquilerActivo = vehiculoTieneAlquilerActivo(v.patente);
    if (tieneAlquilerActivo) {
        printf("[AVISO] El vehiculo tiene un alquiler ACTIVO. Su estado no puede ser alterado.\n");
    }

    char temp[LON_MARCA];
    printf("Nueva marca (actual: %s, ENTER para mantener): ", v.marca);
    if (leerCadena("", temp, LON_MARCA, 1) && strlen(temp) > 0) {
        strcpy(v.marca, temp);
    }

    printf("Nuevo modelo (actual: %s, ENTER para mantener): ", v.modelo);
    if (leerCadena("", temp, LON_MODELO, 1) && strlen(temp) > 0) {
        strcpy(v.modelo, temp);
    }

    int anioMax = (fechaActual() / 10000) + 1;
    printf("Nuevo anio (actual: %d, 0 para mantener): ", v.anio);
    int nuevoAnio = 0;
    leerEntero("", 0, anioMax, &nuevoAnio);
    if (nuevoAnio >= 1900) {
        v.anio = nuevoAnio;
    }

    printf("Nuevo tipo (actual: %s, ENTER para mantener): ", v.tipo);
    if (leerCadena("", temp, LON_TIPO_VEH, 1) && strlen(temp) > 0) {
        strcpy(v.tipo, temp);
    }

    printf("Nueva capacidad (actual: %d personas, 0 para mantener): ", v.capacidad);
    int nuevaCap = 0;
    leerEntero("", 0, 100, &nuevaCap);
    if (nuevaCap >= 1) {
        v.capacidad = nuevaCap;
    }

    printf("Nuevo precio por dia (actual: $%.2f, 0 para mantener): ", v.precio_dia);
    double nuevoPrecio = 0.0;
    leerDouble("", -0.01, &nuevoPrecio);
    if (nuevoPrecio > 0.0) {
        v.precio_dia = nuevoPrecio;
    }

    if (!tieneAlquilerActivo) {
        printf("\nSeleccione el nuevo estado del vehiculo:\n");
        printf("1. %s\n", EST_VEH_DISPONIBLE);
        printf("2. %s\n", EST_VEH_BAJA);
        printf("3. %s\n", EST_VEH_MANT);
        printf("0. Mantener estado actual (%s)\n", v.estado);
        int opcEst = 0;
        leerEntero("Opcion: ", 0, 3, &opcEst);
        switch (opcEst) {
            case 1: strcpy(v.estado, EST_VEH_DISPONIBLE); break;
            case 2: strcpy(v.estado, EST_VEH_BAJA); break;
            case 3: strcpy(v.estado, EST_VEH_MANT); break;
            default: break;
        }
    }

    if (actualizarVehiculo(&v)) {
        printf("\n>>> Vehiculo modificado exitosamente!\n");
    } else {
        printf("\n[ERROR] No se pudieron guardar las modificaciones del vehiculo.\n");
    }
    pausarPantalla();
}

void buscarVehiculo(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("            BUSCAR VEHICULO\n");
    printf("========================================\n");

    char pat[LON_PATENTE];
    leerPatente("Ingrese la patente a buscar: ", pat);

    struct Vehiculo v;
    if (!buscarVehiculoPorPatente(pat, &v)) {
        printf("\n[ERROR] Vehiculo con patente %s no fue encontrado.\n", pat);
        pausarPantalla();
        return;
    }

    printf("\n========================================\n");
    printf("           DATOS DEL VEHICULO\n");
    printf("========================================\n");
    printf("Patente:        %s\n", v.patente);
    printf("Marca y Modelo: %s %s\n", v.marca, v.modelo);
    printf("Anio:           %d\n", v.anio);
    printf("Tipo:           %s\n", v.tipo);
    printf("Capacidad:      %d personas\n", v.capacidad);
    printf("Precio por dia: $%.2f\n", v.precio_dia);
    printf("Estado:         %s\n", v.estado);
    printf("========================================\n");
    pausarPantalla();
}

void listarVehiculos(void) {
    limpiarPantalla();
    printf("\n========================================================================================\n");
    printf("                         LISTADO GENERAL DE VEHICULOS\n");
    printf("========================================================================================\n");

    FILE *f = fopen(ARCH_VEHICULOS, "rb");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s o no hay registros.\n", ARCH_VEHICULOS);
        pausarPantalla();
        return;
    }

    printf("%-10s %-12s %-12s %-6s %-10s %-6s %-12s %-16s\n",
           "PATENTE", "MARCA", "MODELO", "ANIO", "TIPO", "CAP.", "PRECIO/DIA", "ESTADO");
    printf("----------------------------------------------------------------------------------------\n");

    struct Vehiculo v;
    int cont = 0;
    while (fread(&v, sizeof(struct Vehiculo), 1, f) == 1) {
        printf("%-10s %-12s %-12s %-6d %-10s %-6d $%10.2f %-16s\n",
               v.patente, v.marca, v.modelo, v.anio, v.tipo, v.capacidad, v.precio_dia, v.estado);
        cont++;
    }
    fclose(f);

    if (cont == 0) {
        printf("No hay vehiculos registrados.\n");
    } else {
        printf("\nTotal de vehiculos registrados: %d\n", cont);
    }
    pausarPantalla();
}

void submenuVehiculos(void) {
    int opc = -1;
    do {
        limpiarPantalla();
        printf("\n========================================\n");
        printf("           SUBMENU VEHICULOS\n");
        printf("========================================\n");
        printf("1. Registrar vehiculo\n");
        printf("2. Modificar vehiculo\n");
        printf("3. Buscar vehiculo por patente\n");
        printf("4. Listar vehiculos\n");
        printf("0. Volver al Menu Principal\n");
        printf("========================================\n");
        leerEntero("Seleccione una opcion: ", 0, 4, &opc);

        switch (opc) {
            case 1:
                registrarVehiculo();
                break;
            case 2:
                modificarVehiculo();
                break;
            case 3:
                buscarVehiculo();
                break;
            case 4:
                listarVehiculos();
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
