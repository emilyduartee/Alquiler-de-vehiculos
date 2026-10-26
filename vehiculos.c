#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vehiculos.h"
#include "menu.h"
void submenuVehiculos() {
    int opc;
    do {
    	system("cls");
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
            case 4:
                system("cls");
                listarVehiculos();
                pausarPantalla();
                break;
            case 0:
            	system("cls");
                printf("\n>>> Volviendo al Menu Principal...\n");
                break;
            default:
            	system("cls");
                printf("\n>>> ERROR: Opcion no valida.\n");
                break;
        }
    } while (opc != 0);
}

void registrarVehiculo() {
    struct Vehiculo nuevoVehiculo; // datos del auto nuevo
    struct Vehiculo vehiculoLeido; // para leer el archivo
    FILE *archivo;
    int existe = 0;            // 0="no existe", 1="ya existe"
    system("cls");
    printf("\n--- ALTA DE VEHICULO ---\n");
    printf("Ingrese la patente: ");
    fgets(nuevoVehiculo.patente, sizeof(nuevoVehiculo.patente), stdin);
    nuevoVehiculo.patente[strcspn(nuevoVehiculo.patente, "\n")]='\0';
    //Control de patente duplicada
    archivo = fopen("vehiculos.dat", "rb"); 
    if (archivo != NULL) {
        while (fread(&vehiculoLeido, sizeof(struct Vehiculo), 1, archivo)) {
            // se compara la patente guardada con la recien ingresada
            if (strcmp(vehiculoLeido.patente, nuevoVehiculo.patente) == 0) {
                existe = 1; 
                break;
            }
        }
        fclose(archivo);
    }
    if (existe == 1) {
        printf("\n[ERROR] La patente %s ya se encuentra registrada.\n", nuevoVehiculo.patente);
        printf("Operacion cancelada.\n");
    } else {
        printf("Ingrese la marca: ");
        fgets(nuevoVehiculo.marca, sizeof(nuevoVehiculo.marca), stdin);
    	nuevoVehiculo.marca[strcspn(nuevoVehiculo.marca, "\n")]='\0';
        printf("Ingrese el modelo: ");
        fgets(nuevoVehiculo.modelo, sizeof(nuevoVehiculo.modelo), stdin);
    	nuevoVehiculo.modelo[strcspn(nuevoVehiculo.modelo, "\n")]='\0';
        printf("Ingrese el anio: ");
        scanf("%d", &nuevoVehiculo.anio);
        do{
        	printf("Ingrese el anio: ");
        	scanf("%d", &nuevoVehiculo.anio);
        	if (nuevoVehiculo.anio<1900||nuevoVehiculo.anio>2026){
        		printf("ERROR ingrese un año valido. Reintente.");
			}
		} while (nuevoVehiculo.anio<1900||nuevoVehiculo.anio>2026);
        printf("Ingrese el tipo (ej. Auto, Camioneta): ");
        fgets(nuevoVehiculo.tipo, sizeof(nuevoVehiculo.tipo), stdin);
    	nuevoVehiculo.tipo[strcspn(nuevoVehiculo.tipo, "\n")]='\0';
        do{
        	printf("Ingrese la capacidad (cantidad de personas): ");
        	scanf("%d", &nuevoVehiculo.capacidad);
        	if(nuevoVehiculo.capacidad<1){
        		printf("ERROR ingrese una capacidad valida. Reintente.");
			}
		} while (nuevoVehiculo.capacidad<1);
		do{
			printf("Ingrese el precio por dia: ");
        	scanf("%f", &nuevoVehiculo.precio_dia);
        	if (nuevoVehiculo.precio_dia<=0){
        		printf("ERROR ingrese un precio valido. Reintente.");
			}
		} while (nuevoVehiculo.precio_dia<=0);
        strcpy(nuevoVehiculo.estado, "Disponible"); 
        archivo = fopen("vehiculos.dat", "ab"); 
        if (archivo != NULL) {
            fwrite(&nuevoVehiculo, sizeof(struct Vehiculo), 1, archivo);
            fclose(archivo);
            printf("\n>>> Vehiculo registrado exitosamente!\n");
        } else {
            printf("\n[ERROR] No se pudo crear o abrir el archivo vehiculos.dat\n");
        }
    }
}

void modificarVehiculo() {
    FILE *archivo;
    struct Vehiculo vehiculoLeido;
    char patenteBuscada[15];
    int encontrado = 0;
    system("cls");
    printf("\n--- MODIFICAR VEHICULO ---\n");
    printf("Ingrese la patente del vehiculo a modificar: ");
    fgets(patenteBuscada, sizeof(patenteBuscada), stdin);
    patenteBuscada[strcspn(patenteBuscada, "\n")]='\0';
    archivo = fopen("vehiculos.dat", "r+b"); 
    if (archivo == NULL) {
        printf("[ERROR] El archivo no existe o no hay vehiculos registrados.\n");
        return;
    }
    while (fread(&vehiculoLeido, sizeof(struct Vehiculo), 1, archivo)) {
        if (strcmp(vehiculoLeido.patente, patenteBuscada) == 0) {
            encontrado = 1;
            printf("\nVehiculo encontrado: %s %s\n", vehiculoLeido.marca, vehiculoLeido.modelo);
            do{
            	printf("Ingrese el nuevo precio por dia: ");
            	scanf("%f", &vehiculoLeido.precio_dia);
            	if(vehiculoLeido.precio_dia<=0){
            		printf("ERROR ingrese un precio valido. Reintente.");
				}
			} while (vehiculoLeido.precio_dia<=0);
			fflush(stdin);
            printf("Ingrese el nuevo estado (Disponible, Alquilado, Baja, En mantenimiento): ");
            fgets(vehiculoLeido.estado, sizeof(vehiculoLeido.estado), stdin);
    		vehiculoLeido.estado[strcspn(vehiculoLeido.estado, "\n")]='\0';
            fseek(archivo, -sizeof(struct Vehiculo), SEEK_CUR);
            fwrite(&vehiculoLeido, sizeof(struct Vehiculo), 1, archivo);
            printf("\nVehiculo modificado exitosamente!\n");
            break;
        }
    }
    if (encontrado == 0) {
        printf("[ERROR] Vehiculo con patente %s no encontrado.\n", patenteBuscada);
    }
    fclose(archivo);
}

void buscarVehiculo() {
    FILE *archivo;
    struct Vehiculo vehiculoLeido;
    char patenteBuscada[15];
    int encontrado = 0;
    system("cls");
    printf("\n--- BUSCAR VEHICULO ---\n");
    printf("Ingrese la patente a buscar: ");
    fgets(patenteBuscada, sizeof(patenteBuscada), stdin);
    patenteBuscada[strcspn(patenteBuscada, "\n")]='\0';
    archivo = fopen("vehiculos.dat", "rb"); 
    if (archivo == NULL) {
        printf("[ERROR] No hay vehiculos registrados.\n");
        return;
    }
    while (fread(&vehiculoLeido, sizeof(struct Vehiculo), 1, archivo)) {
        if (strcmp(vehiculoLeido.patente, patenteBuscada) == 0) {
            encontrado = 1;
            printf("\n=== DATOS DEL VEHICULO ===\n");
            printf("Patente: %s\n", vehiculoLeido.patente);
            printf("Marca y Modelo: %s %s\n", vehiculoLeido.marca, vehiculoLeido.modelo);
            printf("Anio: %d\n", vehiculoLeido.anio);
            printf("Tipo: %s\n", vehiculoLeido.tipo);
            printf("Capacidad: %d personas\n", vehiculoLeido.capacidad);
            printf("Precio por dia: $%.2f\n", vehiculoLeido.precio_dia);
            printf("Estado: %s\n", vehiculoLeido.estado);
            printf("==========================\n");
            break; //
        }
    }
    if (encontrado == 0) {
        printf("[ERROR] Vehiculo no encontrado.\n");
    }
    fclose(archivo);
}
void listarVehiculos() {
    FILE *archivo;
    struct Vehiculo vehiculoLeido;
    int cont = 0;
    system("cls");
    printf("\n--- LISTADO GENERAL DE VEHICULOS ---\n");
    archivo = fopen("vehiculos.dat", "rb");
    if (archivo == NULL) {
        printf("[ERROR] No hay vehiculos registrados.\n");
        return;
    }
    printf("%-10s %-12s %-12s %-6s %-10s %-6s %-10s %-15s\n", 
           "PATENTE", "MARCA", "MODELO", "ANIO", "TIPO", "CAP.", "PRECIO/DIA", "ESTADO");
    printf("----------------------------------------------------------------------------------------\n");
    while (fread(&vehiculoLeido, sizeof(struct Vehiculo), 1, archivo) == 1) {
        printf("%-10s %-12s %-12s %-6d %-10s %-6d $%-9.2f %-15s\n",
               vehiculoLeido.patente, vehiculoLeido.marca, vehiculoLeido.modelo, 
               vehiculoLeido.anio, vehiculoLeido.tipo, vehiculoLeido.capacidad, 
               vehiculoLeido.precio_dia, vehiculoLeido.estado);
        cont++;
    }
    if (cont == 0) {
        printf("El archivo esta vacio.\n");
    } else {
        printf("\nTotal de vehiculos registrados: %d\n", cont);
    }
    fclose(archivo);
}