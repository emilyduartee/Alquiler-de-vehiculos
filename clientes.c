#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "clientes.h"
#include "comun.h"
#include "utils.h"
#include "fechas.h"
#include "alquileres.h"

int buscarClientePorDNI(int dni, struct Cliente *out) {
    FILE *f = fopen(ARCH_CLIENTES, "rb");
    if (!f) {
        return 0;
    }
    struct Cliente c;
    int encontrado = 0;
    while (fread(&c, sizeof(struct Cliente), 1, f) == 1) {
        if (c.DNI == dni) {
            if (out) {
                *out = c;
            }
            encontrado = 1;
            break;
        }
    }
    fclose(f);
    return encontrado;
}

int existeCliente(int dni) {
    return buscarClientePorDNI(dni, NULL);
}

int guardarCliente(const struct Cliente *c) {
    FILE *f = fopen(ARCH_CLIENTES, "ab");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s para escritura.\n", ARCH_CLIENTES);
        return 0;
    }
    size_t escritos = fwrite(c, sizeof(struct Cliente), 1, f);
    fclose(f);
    return escritos == 1;
}

int actualizarCliente(const struct Cliente *c) {
    FILE *f = fopen(ARCH_CLIENTES, "rb+");
    if (!f) {
        printf("[ERROR] No se pudo abrir %s para actualizacion.\n", ARCH_CLIENTES);
        return 0;
    }
    struct Cliente leido;
    int actualizado = 0;
    while (fread(&leido, sizeof(struct Cliente), 1, f) == 1) {
        if (leido.DNI == c->DNI) {
            if (fseek(f, -(long)sizeof(struct Cliente), SEEK_CUR) != 0) {
                break;
            }
            if (fwrite(c, sizeof(struct Cliente), 1, f) == 1) {
                actualizado = 1;
            }
            break;
        }
    }
    fclose(f);
    return actualizado;
}

void registrarCliente(void) {
    struct Cliente nuevo;
    memset(&nuevo, 0, sizeof(struct Cliente));

    limpiarPantalla();
    printf("\n========================================\n");
    printf("           ALTA DE CLIENTE\n");
    printf("========================================\n");

    leerDNI("Ingrese el DNI del cliente: ", &nuevo.DNI);

    /* Validacion de DNI unico (B1) */
    if (existeCliente(nuevo.DNI)) {
        printf("\n[ERROR] El DNI %d ya se encuentra registrado en el sistema.\n", nuevo.DNI);
        printf("Operacion cancelada.\n");
        pausarPantalla();
        return;
    }

    leerCadena("Ingrese Apellido y Nombre: ", nuevo.apellidoNombre, LON_NOMBRE, 0);
    leerCadena("Ingrese Telefono: ", nuevo.telefono, LON_TEL, 0);
    leerCadena("Ingrese E-mail: ", nuevo.email, LON_EMAIL, 0);
    leerCadena("Ingrese Direccion: ", nuevo.direccion, LON_DIR, 0);

    int fechaIngresada = 0;
    printf("Fecha de alta (presione ENTER o ingrese 0 para usar la fecha actual):\n");
    leerEntero("Ingrese Fecha de alta (AAAAMMDD): ", 0, 20991231, &fechaIngresada);
    if (fechaIngresada == 0 || !esFechaValida(fechaIngresada)) {
        nuevo.fechaAlta = fechaActual();
    } else {
        nuevo.fechaAlta = fechaIngresada;
    }

    nuevo.estado = CLI_ACTIVO;

    if (guardarCliente(&nuevo)) {
        printf("\n>>> Cliente registrado exitosamente (Estado: ACTIVO).\n");
    } else {
        printf("\n[ERROR] No se pudo guardar el cliente en el archivo.\n");
    }
    pausarPantalla();
}

void modificarCliente(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("         MODIFICAR CLIENTE\n");
    printf("========================================\n");

    int dniBuscado = 0;
    leerDNI("Ingrese el DNI del cliente a modificar: ", &dniBuscado);

    struct Cliente c;
    if (!buscarClientePorDNI(dniBuscado, &c)) {
        printf("\n[ERROR] Cliente con DNI %d no fue encontrado.\n", dniBuscado);
        pausarPantalla();
        return;
    }

    printf("\nCliente encontrado: %s\n", c.apellidoNombre);
    printf("Estado actual: %s\n", (c.estado == CLI_ACTIVO) ? "ACTIVO" : "INACTIVO");
    printf("----------------------------------------\n");

    char temp[LON_EMAIL];
    printf("Ingrese nuevo Telefono (actual: %s, ENTER para mantener): ", c.telefono);
    if (leerCadena("", temp, LON_TEL, 1) && strlen(temp) > 0) {
        strcpy(c.telefono, temp);
    }

    printf("Ingrese nuevo E-mail (actual: %s, ENTER para mantener): ", c.email);
    if (leerCadena("", temp, LON_EMAIL, 1) && strlen(temp) > 0) {
        strcpy(c.email, temp);
    }

    printf("Ingrese nueva Direccion (actual: %s, ENTER para mantener): ", c.direccion);
    if (leerCadena("", temp, LON_DIR, 1) && strlen(temp) > 0) {
        strcpy(c.direccion, temp);
    }

    printf("\nModificar estado (1: Activo, 0: Inactivo, actual: %d):\n", c.estado);
    int nuevoEstado = c.estado;
    leerEntero("Nuevo estado (1/0): ", 0, 1, &nuevoEstado);
    c.estado = nuevoEstado;

    if (actualizarCliente(&c)) {
        printf("\n>>> Datos del cliente modificados exitosamente.\n");
    } else {
        printf("\n[ERROR] No se pudieron guardar los cambios del cliente.\n");
    }
    pausarPantalla();
}

static void mostrarHistorialTxtCliente(int dni) {
    char nombreTxt[64];
    sprintf(nombreTxt, "movimientos_%d.txt", dni);
    FILE *fTxt = fopen(nombreTxt, "r");
    if (!fTxt) {
        printf("\n[AVISO] El cliente no registra movimientos en archivo de texto.\n");
        return;
    }
    printf("\n--- HISTORIAL DE MOVIMIENTOS (archivo %s) ---\n", nombreTxt);
    printf("%-6s | %-6s | %-8s | %-12s | %-10s | %-10s | %-12s\n",
           "NRO", "ALQ", "TIPO", "MONTO", "FECHA", "PATENTE", "MEDIO");
    printf("-------------------------------------------------------------------------\n");
    char linea[256];
    int lineas = 0;
    while (fgets(linea, sizeof(linea), fTxt)) {
        linea[strcspn(linea, "\r\n")] = '\0';
        if (strlen(linea) == 0) continue;
        int nMov = 0, nAlq = 0, fch = 0, d = 0;
        double mto = 0.0;
        char tipo[LON_TIPO_MOV], pat[LON_PATENTE], med[LON_MEDIO_PAGO];
        /* Formato CSV ;: numMov;numAlq;Tipo;monto;fecha;DNI;patente;medio */
        if (sscanf(linea, "%d;%d;%9[^;];%lf;%d;%d;%14[^;];%19[^\n]",
                   &nMov, &nAlq, tipo, &mto, &fch, &d, pat, med) >= 7) {
            printf("%-6d | %-6d | %-8s | $%11.2f | %-10d | %-10s | %-12s\n",
                   nMov, nAlq, tipo, mto, fch, pat, med);
            lineas++;
        }
    }
    fclose(fTxt);
    if (lineas == 0) {
        printf("Sin registros legibles en el historial.\n");
    }
}

static void mostrarAlquileresCliente(int dni) {
    FILE *f = fopen(ARCH_ALQUILERES, "rb");
    if (!f) {
        return;
    }
    struct Alquiler a;
    int cant = 0;
    printf("\n--- HISTORIAL DE ALQUILERES DEL CLIENTE ---\n");
    while (fread(&a, sizeof(struct Alquiler), 1, f) == 1) {
        if (a.DNI == dni) {
            if (cant == 0) {
                imprimirEncabezadoAlquileres();
            }
            imprimirFilaAlquiler(&a);
            cant++;
        }
    }
    fclose(f);
    if (cant == 0) {
        printf("El cliente no posee alquileres registrados.\n");
    } else {
        printf("Total de alquileres registrados: %d\n", cant);
    }
}

void consultarCliente(void) {
    limpiarPantalla();
    printf("\n========================================\n");
    printf("         CONSULTAR CLIENTE\n");
    printf("========================================\n");

    int dniBuscado = 0;
    leerDNI("Ingrese el DNI del cliente a consultar: ", &dniBuscado);

    struct Cliente c;
    if (!buscarClientePorDNI(dniBuscado, &c)) {
        printf("\n[ERROR] Cliente con DNI %d no fue encontrado.\n", dniBuscado);
        pausarPantalla();
        return;
    }

    printf("\n========================================\n");
    printf("           DATOS DEL CLIENTE\n");
    printf("========================================\n");
    printf("DNI:            %d\n", c.DNI);
    printf("Apellido y Nom: %s\n", c.apellidoNombre);
    printf("Telefono:       %s\n", c.telefono);
    printf("E-mail:         %s\n", c.email);
    printf("Direccion:      %s\n", c.direccion);
    printf("Fecha de Alta:  %d\n", c.fechaAlta);
    printf("Estado:         %s\n", (c.estado == CLI_ACTIVO) ? "ACTIVO" : "INACTIVO");
    printf("========================================\n");

    /* A12: Historial de alquileres y movimientos */
    mostrarAlquileresCliente(c.DNI);
    mostrarHistorialTxtCliente(c.DNI);

    pausarPantalla();
}

void buscarCliente(void) {
    consultarCliente();
}

void submenuClientes(void) {
    int opc = -1;
    do {
        limpiarPantalla();
        printf("\n========================================\n");
        printf("           SUBMENU CLIENTES\n");
        printf("========================================\n");
        printf("1. Registrar cliente\n");
        printf("2. Modificar cliente\n");
        printf("3. Consultar cliente (con historial)\n");
        printf("0. Volver al Menu Principal\n");
        printf("========================================\n");
        leerEntero("Seleccione una opcion: ", 0, 3, &opc);

        switch (opc) {
            case 1:
                registrarCliente();
                break;
            case 2:
                modificarCliente();
                break;
            case 3:
                consultarCliente();
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
