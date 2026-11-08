#include <stdio.h>
#include <stdlib.h>
#include "menu.h"
#include "vehiculos.h"
#include "clientes.h"
#include "movimientos.h"
#include "alquileres.h"
#include "reportes.h"
#include "utils.h"

void mostrarMenuPrincipal(void) {
    int opc = -1;

    do {
        limpiarPantalla();
        printf("========================================\n");
        printf("   SISTEMA DE ALQUILER DE VEHICULOS\n");
        printf("========================================\n");
        printf("1. Clientes\n");
        printf("2. Vehiculos\n");
        printf("3. Alquileres\n");
        printf("4. Devoluciones\n");
        printf("5. Pagos\n");
        printf("6. Consultas y Reportes\n");
        printf("0. Salir\n");
        printf("========================================\n");
        leerEntero("Seleccione una opcion: ", 0, 6, &opc);

        switch (opc) {
            case 1:
                submenuClientes();
                break;
            case 2:
                submenuVehiculos();
                break;
            case 3:
                submenuAlquileres();
                break;
            case 4:
                registrarDevolucion();
                pausarPantalla();
                break;
            case 5:
                submenuMovimientos();
                break;
            case 6:
                submenuReportes();
                break;
            case 0:
                limpiarPantalla();
                printf("\n>>> Saliendo del sistema...\n");
                break;
            default:
                printf("\n[ERROR] Opcion no valida.\n");
                pausarPantalla();
                break;
        }
    } while (opc != 0);
}
