#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vehiculos.h"
#include "menu.h"
void submenuVehiculos() {
    int opc;
    do {
        printf("\n=== SUBMENU VEHICULOS ===\n");
        printf("1. Registrar vehiculo\n");
        printf("2. Modificar vehiculo\n");
        printf("3. Buscar vehiculo\n");
        printf("0. Volver al Menu Principal\n");
        printf("Ingrese una opcion: ");
        scanf("%d", &opc);
        
        switch(opc) {
            case 1:
            	system("cls");
                registrarVehiculo();
                pausarPantalla();
                break;
            case 2:
            	system("cls");
                modificarVehiculo();
                pausarPantalla();
                break;
            case 3:
            	system("cls");
                buscarVehiculo();
                pausarPantalla();
                break;
            case 0:
            	system("cls");
                printf("\n>>> Volviendo al Menu Principal...\n");
                pausarPantalla();
                break;
            default:
            	system("cls");
                printf("\n>>> ERROR: Opcion no valida.\n");
                break;
        }
    } while (opc != 0);
}

void registrarVehiculo() {
    struct Vehiculo nuevoAuto; // Variable para guardar los datos del auto nuevo.
    struct Vehiculo autoLeido; // Variable temporal para leer el archivo.
    FILE *archivo;
    int existe = 0;            // 0="no existe", 1="ya existe".
    printf("\n--- ALTA DE VEHICULO ---\n");
    printf("Ingrese la patente: ");
    scanf("%s", nuevoAuto.patente);
    //Control de patente duplicada
    archivo = fopen("vehiculos.dat", "rb"); 
    if (archivo != NULL) {
        while (fread(&autoLeido, sizeof(struct Vehiculo), 1, archivo)) {
            // se compara la patente guardada con la recien ingresada
            if (strcmp(autoLeido.patente, nuevoAuto.patente) == 0) {
                existe = 1; 
                break;
            }
        }
        fclose(archivo);
    }
    if (existe == 1) {
        printf("\n[ERROR] La patente %s ya se encuentra registrada.\n", nuevoAuto.patente);
        printf("Operacion cancelada.\n");
    } else {
        printf("Ingrese la marca: ");
        scanf("%s", nuevoAuto.marca);
        printf("Ingrese el modelo: ");
        scanf("%s", nuevoAuto.modelo);
        printf("Ingrese el anio: ");
        scanf("%d", &nuevoAuto.anio);
        printf("Ingrese el tipo (ej. Auto, Camioneta): ");
        scanf("%s", nuevoAuto.tipo);
        printf("Ingrese la capacidad (cantidad de personas): ");
        scanf("%d", &nuevoAuto.capacidad);
        printf("Ingrese el precio por dia: ");
        scanf("%f", &nuevoAuto.precio_dia);    
        strcpy(nuevoAuto.estado, "Disponible"); 
        archivo = fopen("vehiculos.dat", "ab"); 
        if (archivo != NULL) {
            fwrite(&nuevoAuto, sizeof(struct Vehiculo), 1, archivo);
            fclose(archivo);
            printf("\n>>> Vehiculo registrado exitosamente!\n");
        } else {
            printf("\n[ERROR] No se pudo crear o abrir el archivo vehiculos.dat\n");
        }
    }
}

void modificarVehiculo() {
    FILE *archivo;
    struct Vehiculo autoLeido;
    char patenteBuscada[15];
    int encontrado = 0;
    printf("\n--- MODIFICAR VEHICULO ---\n");
    printf("Ingrese la patente del vehiculo a modificar: ");
    scanf("%s", patenteBuscada);
    archivo = fopen("vehiculos.dat", "r+b"); 
    if (archivo == NULL) {
        printf("[ERROR] El archivo no existe o no hay vehiculos registrados.\n");
        return;
    }
    while (fread(&autoLeido, sizeof(struct Vehiculo), 1, archivo)) {
        if (strcmp(autoLeido.patente, patenteBuscada) == 0) {
            encontrado = 1;
            printf("\nVehiculo encontrado: %s %s\n", autoLeido.marca, autoLeido.modelo);
            printf("Ingrese el nuevo precio por dia: ");
            scanf("%f", &autoLeido.precio_dia);
            printf("Ingrese el nuevo estado (Disponible, Alquilado, Baja, En mantenimiento): ");
            scanf("%s", autoLeido.estado);
            fseek(archivo, -sizeof(struct Vehiculo), SEEK_CUR);
            fwrite(&autoLeido, sizeof(struct Vehiculo), 1, archivo);
            printf("\nVehiculo modificado exitosamente!\n");
        }
    }
    if (encontrado == 0) {
        printf("[ERROR] Vehiculo con patente %s no encontrado.\n", patenteBuscada);
    }
    fclose(archivo);
}

void buscarVehiculo() {
    FILE *archivo;
    struct Vehiculo autoLeido;
    char patenteBuscada[15];
    int encontrado = 0;
    printf("\n--- BUSCAR VEHICULO ---\n");
    printf("Ingrese la patente a buscar: ");
    scanf("%s", patenteBuscada);
    archivo = fopen("vehiculos.dat", "rb"); 
    if (archivo == NULL) {
        printf("[ERROR] No hay vehiculos registrados.\n");
        return;
    }
    while (fread(&autoLeido, sizeof(struct Vehiculo), 1, archivo)) {
        if (strcmp(autoLeido.patente, patenteBuscada) == 0) {
            encontrado = 1;
            printf("\n=== DATOS DEL VEHICULO ===\n");
            printf("Patente: %s\n", autoLeido.patente);
            printf("Marca y Modelo: %s %s\n", autoLeido.marca, autoLeido.modelo);
            printf("Anio: %d\n", autoLeido.anio);
            printf("Tipo: %s\n", autoLeido.tipo);
            printf("Capacidad: %d personas\n", autoLeido.capacidad);
            printf("Precio por dia: $%.2f\n", autoLeido.precio_dia);
            printf("Estado: %s\n", autoLeido.estado);
            printf("==========================\n");
            break; //
        }
    }
    if (encontrado == 0) {
        printf("[ERROR] Vehiculo no encontrado.\n");
    }
    fclose(archivo);
}