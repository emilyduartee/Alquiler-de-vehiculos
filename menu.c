#include <stdio.h>
#include <stdlib.h>
#include "menu.h" 

void pausarPantalla() {
    printf("\nPresione ENTER para continuar...");
    // Limpieza de buffer
    while(getchar() != '\n');
    getchar(); // Espera a que el usuario presione una tecla
}

// Desarrollo de la función menu
void mostrarMenuPrincipal() {
    int opc;
    
    do {
    	system("cls"); // Limpieza de pantalla
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Clientes\n");
        printf("2. Vehiculos\n");
        printf("3. Alquileres\n");
        printf("4. Devoluciones\n");
        printf("5. Pagos\n");
        printf("6. Consultas y Reportes\n");
        printf("0. Salir\n");
        printf("Ingrese una opcion: ");
        scanf("%d", &opc);
        
        switch(opc) {
            case 1:
                system("cls"); // Limpiamos antes de entrar al módulo
                printf("--- GESTION DE CLIENTES ---\n");
                // Funciones de clientes
                pausarPantalla(); // Pausamos para que el usuario lea
                break;
            case 2:
                system("cls");
                printf("--- GESTION DE VEHICULOS ---\n");
                pausarPantalla();
                break;
            case 3:
                system("cls");
                printf("--- GESTION DE ALQUILERES ---\n");
                pausarPantalla();
                break;
            case 4:
                system("cls");
                printf("--- GESTION DE DEVOLUCIONES ---\n");
                pausarPantalla();
                break;
            case 5:
               system("cls");
                printf("--- GESTION DE PAGOS ---\n");
                pausarPantalla();
                break;
            case 6:
                system("cls");
                printf("--- CONSULTAS Y REPORTES ---\n");
                pausarPantalla();
                break;
            case 0:
                system("cls");
                printf("--- SALIENDO DEL PROGRAMA ---\n");
                break;
            default:
               system("cls");
                printf("--- ERROR: Opcion no valida ---\n");
                printf("--- Ingrese una opcion valida ---\n");
                pausarPantalla();
                break;
        }
    } while (opc != 0);
}