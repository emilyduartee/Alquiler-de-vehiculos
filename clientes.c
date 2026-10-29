#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clientes.h"
#include "menu.h"

int esFechaValida(long fecha) {
    int anio = fecha / 10000;
    int mes = (fecha % 10000) / 100;
    int dia = fecha % 100;
    if (anio < 1900 || anio > 2026) return 0;
    if (mes < 1 || mes > 12) return 0;
    if (dia < 1 || dia > 31) return 0;
    if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && dia > 30) return 0;
    if (mes == 2) {
        int esBisiesto = (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
        if (esBisiesto && dia > 29) return 0;
        if (!esBisiesto && dia > 28) return 0;
    }
    return 1;
}

void submenuClientes() {
    int opc;
    do {
        system("cls");
        printf("\n=== SUBMENU CLIENTES ===\n");
        printf("1. Registrar cliente\n");
        printf("2. Modificar cliente\n");
        printf("3. Consultar datos de cliente\n");
        printf("0. Volver al Menu Principal\n");
        printf("Ingrese una opcion: ");
        scanf("%d", &opc);
        switch(opc) {
            case 1:
                system("cls");
                registrarCliente();
                pausarPantalla();
                break;
            case 2:
                system("cls");
                modificarCliente();
                pausarPantalla();
                break;
            case 3:
                system("cls");
                buscarCliente();
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

void registrarCliente() {
    struct Cliente nuevoCliente;
    struct Cliente clienteLeido;
    FILE *archivo;
    int existe=0;
    system("cls");
    printf("\n--- ALTA DE CLIENTE ---\n");
    do {
        printf("Ingrese el DNI del cliente: ");
        if (scanf("%ld", &nuevoCliente.DNI) != 1) {
            printf("ERROR debe ingresar solo numeros. Reintente.\n");
            fflush(stdin);
            nuevoCliente.DNI = 0;
        } else if (nuevoCliente.DNI <= 0) {
            printf("ERROR ingrese un DNI valido (mayor a cero). Reintente.\n");
        }
    } while (nuevoCliente.DNI <= 0);
    if (archivo!=NULL) {
        while (fread(&clienteLeido, sizeof(struct Cliente), 1, archivo) == 1) {
            if (clienteLeido.DNI == nuevoCliente.DNI) {
                existe=1;
                break;
            }
        }
        fclose(archivo);
    }
    if (existe == 1) {
        printf("\nERROR el DNI %ld ya se encuentra registrado en el sistema.\n", nuevoCliente.DNI);
        printf("Operacion cancelada.\n");
    } else {
        fflush(stdin);
        printf("Ingrese Apellido y Nombre: ");
        fgets(nuevoCliente.apellidoNombre, sizeof(nuevoCliente.apellidoNombre), stdin);
        nuevoCliente.apellidoNombre[strcspn(nuevoCliente.apellidoNombre, "\n")] = '\0';
        printf("Ingrese Telefono: ");
        fgets(nuevoCliente.telefono, sizeof(nuevoCliente.telefono), stdin);
        nuevoCliente.telefono[strcspn(nuevoCliente.telefono, "\n")] = '\0';
        printf("Ingrese E-mail: ");
        fgets(nuevoCliente.email, sizeof(nuevoCliente.email), stdin);
        nuevoCliente.email[strcspn(nuevoCliente.email, "\n")] = '\0';
        printf("Ingrese Direccion: ");
        fgets(nuevoCliente.direccion, sizeof(nuevoCliente.direccion), stdin);
        nuevoCliente.direccion[strcspn(nuevoCliente.direccion, "\n")] = '\0';
        do {
            printf("Ingrese Fecha de alta (AAAAMMDD): ");
            scanf("%ld", &nuevoCliente.fechaAlta);
            if (!esFechaValida(nuevoCliente.fechaAlta)) {
                printf("ERROR fecha invalida o incoherente. Reintente.\n");
            }
        } while (!esFechaValida(nuevoCliente.fechaAlta));
        fflush(stdin);
        nuevoCliente.estado = 1;
        archivo = fopen("clientes.dat", "ab");
        if (archivo != NULL) {
            fwrite(&nuevoCliente, sizeof(struct Cliente), 1, archivo);
            fclose(archivo);
            printf("\nCliente activo registrado exitosamente.\n");
        } else {
            printf("\nERROR no se pudo abrir o crear el archivo clientes.dat\n");
        }
    }
}

void modificarCliente() {
    FILE *archivo;
    struct Cliente clienteLeido;
    long dniBuscado;
    int encontrado = 0;
    int opcionEstado;
    system("cls");
    printf("\n--- MODIFICAR CLIENTE ---\n");
    do {
        printf("Ingrese el DNI del cliente: ");
        if (scanf("%ld", &dniBuscado)!=1) {
            printf("ERROR debe ingresar solo numeros. Reintente.\n");
            fflush(stdin);
            dniBuscado=0;
        } else if (dniBuscado<=0){
            printf("ERROR Ingrese un DNI valido (mayor a cero). Reintente.\n");
        }
    } while (dniBuscado<= 0);
    archivo = fopen("clientes.dat", "rb+");
    if (archivo == NULL) {
        printf("ERROR el archivo clientes.dat no existe o no hay registros.\n");
        return;
    }
    while (fread(&clienteLeido, sizeof(struct Cliente), 1, archivo) == 1) {
        if (clienteLeido.DNI == dniBuscado) {
            encontrado = 1;
            printf("\nCliente encontrado: %s (Estado actual: %s)\n", 
                   clienteLeido.apellidoNombre, 
                   (clienteLeido.estado == 1) ? "ACTIVO" : "INACTIVO");
            fflush(stdin);
            printf("Ingrese nuevo Telefono: ");
            fgets(clienteLeido.telefono, sizeof(clienteLeido.telefono), stdin);
            clienteLeido.telefono[strcspn(clienteLeido.telefono, "\n")] = '\0';
            printf("Ingrese nuevo E-mail: ");
            fgets(clienteLeido.email, sizeof(clienteLeido.email), stdin);
            clienteLeido.email[strcspn(clienteLeido.email, "\n")] = '\0';
            printf("Ingrese nueva Direccion: ");
            fgets(clienteLeido.direccion, sizeof(clienteLeido.direccion), stdin);
            clienteLeido.direccion[strcspn(clienteLeido.direccion, "\n")] = '\0';
            printf("Ingrese el nuevo estado (Activo=1, Inactivo=0): ");
            scanf("%d", &opcionEstado);
            if (opcionEstado == 1 || opcionEstado == 0) {
                clienteLeido.estado = opcionEstado;
            } else {
                printf("Opcion invalida. Se mantiene el estado anterior.\n");
            }
            fseek(archivo, -sizeof(struct Cliente), SEEK_CUR);
            fwrite(&clienteLeido, sizeof(struct Cliente), 1, archivo);
            printf("\nDatos del cliente modificados exitosamente.\n");
            break;
        }
    }
    if (!encontrado) {
        printf("ERROR cliente con DNI %ld no encontrado.\n", dniBuscado);
    }
    fclose(archivo);
}

void buscarCliente(){
    FILE *archClientes;
    struct Cliente clienteLeido;
    long dniBuscado;
    int encontrado=0;
    system("cls");
    printf("\n--- BUSCAR CLIENTE POR DNI ---\n");
    do {
        printf("Ingrese el DNI del cliente: ");
        if (scanf("%ld", &dniBuscado)!=1) {
            printf("ERROR debe ingresar solo numeros. Reintente.\n");
            fflush(stdin);
            dniBuscado=0;
        } else if (dniBuscado<=0) {
            printf("ERROR ingrese un DNI valido (mayor a cero). Reintente.\n");
        }
    } while (dniBuscado <= 0);
    archClientes=fopen("clientes.dat", "rb");
    if (archClientes==NULL) {
        printf("[ERROR] No hay clientes registrados.\n");
        return;
    }
    while (fread(&clienteLeido, sizeof(struct Cliente), 1, archClientes)==1) {
        if (clienteLeido.DNI==dniBuscado) {
            encontrado=1;
            printf("\n=== DATOS DEL CLIENTE ===\n");
            printf("DNI:             %ld\n", clienteLeido.DNI);
            printf("Apellido y Nom:  %s\n", clienteLeido.apellidoNombre);
            printf("Telefono:        %s\n", clienteLeido.telefono);
            printf("E-mail:          %s\n", clienteLeido.email);
            printf("Direccion:       %s\n", clienteLeido.direccion);
            printf("Fecha de Alta:   %ld\n", clienteLeido.fechaAlta);
            printf("Estado:          %s\n", (clienteLeido.estado == 1) ? "ACTIVO" : "INACTIVO");
            printf("=========================\n");
            break;
        }
    }
    fclose(archClientes);
    if (!encontrado) {
        printf("ERROR cliente con DNI %ld no fue encontrado.\n", dniBuscado);
    }
}