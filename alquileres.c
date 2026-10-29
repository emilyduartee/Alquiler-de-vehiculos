#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alquileres.h"
#include "clientes.h"
#include "vehiculos.h"
#include "menu.h"
void submenuAlquileres() {
    int opc;
    do {
        system("cls");
        printf("\n=== SUBMENU GESTION DE ALQUILERES ===\n");
        printf("1. Realizar nuevo alquiler\n");
        printf("2. Listar todos los alquileres\n");
        printf("3. Modificar alquiler\n");
        printf("0. Volver al Menu Principal\n");
        printf("Ingrese una opcion: ");
        scanf("%d", &opc);
        
        switch(opc) {
            case 1:
                system("cls");
                realizarAlquiler();
                pausarPantalla();
                break;
            case 2:
                system("cls");
                listarAlquileres();
                pausarPantalla();
                break;
            case 3:
                system("cls");
                modificarAlquiler();
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

void realizarAlquiler() {
    FILE *fClientes, *fVehiculos, *fAlquileres;
    struct Cliente cliente;
    struct Vehiculo vehiculo;
    struct Alquiler nuevoAlquiler, auxAlquiler;
    int clienteValido = 0;
    int vehiculoValido = 0;
    float precioDiaVehiculo = 0.0;
    int ultimoNumero = 0;
    system("cls");
    printf("\n--- REGISTRAR NUEVO ALQUILER ---\n");
    do {
        printf("Ingrese el DNI del cliente: ");
        if (scanf("%ld", &nuevoAlquiler.DNI) != 1) {
            printf("ERROR debe ingresar solo numeros. Reintente.\n");
            fflush(stdin);
            nuevoAlquiler.DNI = 0;
        } else if (nuevoAlquiler.DNI <= 0) {
            printf("ERROR ingrese un DNI valido. Reintente.\n");
        }
    } while (nuevoAlquiler.DNI <= 0);
    fClientes = fopen("clientes.dat", "rb");
    if (fClientes == NULL) {
        printf("[ERROR] No se pudo abrir clientes.dat o no hay clientes registrados.\n");
        return;
    }
    while (fread(&cliente, sizeof(struct Cliente), 1, fClientes) == 1) {
        if (cliente.DNI == nuevoAlquiler.DNI) {
            if (cliente.estado == 1) {
                clienteValido = 1;
                printf("Cliente verificado: %s\n", cliente.apellidoNombre);
            } else {
                printf("[ERROR] El cliente existe pero se encuentra INACTIVO.\n");
                fclose(fClientes);
                return;
            }
            break;
        }
    }
    fclose(fClientes);
    if (!clienteValido) {
        printf("[ERROR] El DNI %ld no esta registrado en el sistema.\n", nuevoAlquiler.DNI);
        return;
    }
    FILE *fCheckAlq = fopen("alquileres.dat", "rb");
    struct Alquiler alqCheck;
    int tieneAlquilerActivo = 0;
    if (fCheckAlq != NULL) {
        while (fread(&alqCheck, sizeof(struct Alquiler), 1, fCheckAlq) == 1) {
            if (alqCheck.DNI == nuevoAlquiler.DNI && strcmp(alqCheck.estado, "Activo") == 0) {
                tieneAlquilerActivo = 1;
                break;
            }
        }
        fclose(fCheckAlq);
    }
    if (tieneAlquilerActivo) {
        printf("\nERROR el cliente DNI %ld ya posee el Alquiler Nro %d en estado ACTIVO.\n", 
               nuevoAlquiler.DNI, alqCheck.numAlquiler);
        printf("Debe registrar la devolucion del vehiculo anterior antes de efectuar una nueva contratacion.\n");
        return;
    }
	fflush(stdin);
    printf("Ingrese la patente del vehiculo: ");
    fgets(nuevoAlquiler.patente, sizeof(nuevoAlquiler.patente), stdin);
    nuevoAlquiler.patente[strcspn(nuevoAlquiler.patente, "\n")] = '\0';
    fVehiculos = fopen("vehiculos.dat", "rb+");
    if (fVehiculos == NULL) {
        printf("[ERROR] No se pudo abrir vehiculos.dat o no hay vehiculos registrados.\n");
        return;
    }
    while (fread(&vehiculo, sizeof(struct Vehiculo), 1, fVehiculos) == 1) {
        if (strcmp(vehiculo.patente, nuevoAlquiler.patente) == 0) {
            if (strcmp(vehiculo.estado, "Disponible") == 0) {
                vehiculoValido = 1;
                precioDiaVehiculo = vehiculo.precio_dia;
                printf("Vehiculo verificado: %s %s (Precio/Dia: $%.2f)\n", 
                       vehiculo.marca, vehiculo.modelo, precioDiaVehiculo);
            } else {
                printf("[ERROR] El vehiculo no esta disponible. Estado actual: %s\n", vehiculo.estado);
                fclose(fVehiculos);
                return;
            }
            break;
        }
    }
    if (!vehiculoValido) {
        printf("[ERROR] La patente %s no existe en el sistema.\n", nuevoAlquiler.patente);
        fclose(fVehiculos);
        return;
    }
    do {
        printf("Ingrese la fecha de inicio (AAAAMMDD): ");
        scanf("%ld", &nuevoAlquiler.fechaInicio);
        if (!esFechaValida(nuevoAlquiler.fechaInicio)) {
            printf("[ERROR] Fecha invalida o incoherente. Reintente.\n");
        }
    } while (!esFechaValida(nuevoAlquiler.fechaInicio));
    do {
        printf("Ingrese la cantidad de dias contratados (> 0): ");
        scanf("%d", &nuevoAlquiler.cantDias);
        if (nuevoAlquiler.cantDias <= 0) {
            printf("[ERROR] La cantidad de dias debe ser mayor a cero.\n");
        }
    } while (nuevoAlquiler.cantDias <= 0);
    nuevoAlquiler.importeTotal = nuevoAlquiler.cantDias * precioDiaVehiculo;
    nuevoAlquiler.importePagado = 0.0;
    nuevoAlquiler.fechaPrevista = nuevoAlquiler.fechaInicio; 
    nuevoAlquiler.fechaEfectiva = 0;                         
    strcpy(nuevoAlquiler.estado, "Activo");
    fAlquileres = fopen("alquileres.dat", "rb");
    if (fAlquileres != NULL) {
        while (fread(&auxAlquiler, sizeof(struct Alquiler), 1, fAlquileres) == 1) {
            ultimoNumero = auxAlquiler.numAlquiler;
        }
        fclose(fAlquileres);
    }
    nuevoAlquiler.numAlquiler = ultimoNumero + 1;
    strcpy(vehiculo.estado, "Alquilado");
    fseek(fVehiculos, -sizeof(struct Vehiculo), SEEK_CUR);
    fwrite(&vehiculo, sizeof(struct Vehiculo), 1, fVehiculos);
    fclose(fVehiculos);
    fAlquileres = fopen("alquileres.dat", "ab");
    if (fAlquileres != NULL) {
        fwrite(&nuevoAlquiler, sizeof(struct Alquiler), 1, fAlquileres);
        fclose(fAlquileres);
        printf("\n==================================================\n");
        printf(">>> ALQUILER REGISTRADO EXITOSAMENTE!\n");
        printf("Nro. de Alquiler:     %d\n", nuevoAlquiler.numAlquiler);
        printf("Importe Total Base:   $%.2f\n", nuevoAlquiler.importeTotal);
        printf("Estado del Vehiculo:  ALQUILADO\n");
        printf("==================================================\n");
    } else {
        printf("[ERROR] No se pudo guardar el alquiler en disco.\n");
    }
}

void registrarDevolucion() {
    FILE *fAlquileres, *fVehiculos;
    struct Alquiler alquiler;
    struct Vehiculo vehiculo;
    int numAlquilerBuscado;
    int encontrado = 0;
    long fechaDevolucion;
    int diasAdicionales = 0;
    float precioDiaVehiculo = 0.0;
    float recargoExtra = 0.0;
    system("cls");
    printf("\n--- REGISTRAR DEVOLUCION DE VEHICULO ---\n");
    printf("Ingrese el Numero de Alquiler a finalizar: ");
    scanf("%d", &numAlquilerBuscado);
    fAlquileres = fopen("alquileres.dat", "rb+");
    if (fAlquileres == NULL) {
        printf("[ERROR] No se pudo abrir el archivo de alquileres o no hay registros.\n");
        return;
    }
    while (fread(&alquiler, sizeof(struct Alquiler), 1, fAlquileres) == 1) {
        if (alquiler.numAlquiler == numAlquilerBuscado) {
            if (strcmp(alquiler.estado, "Activo") != 0) {
                printf("[ERROR] El alquiler Nro %d no esta ACTIVO (Estado actual: %s).\n", 
                       numAlquilerBuscado, alquiler.estado);
                fclose(fAlquileres);
                return;
            }
            encontrado = 1;
            printf("\nAlquiler encontrado:\n");
            printf("Cliente DNI: %ld | Patente: %s | F. Inicio: %ld\n", 
                   alquiler.DNI, alquiler.patente, alquiler.fechaInicio);
            do {
                printf("Ingrese la fecha efectiva de devolucion (AAAAMMDD): ");
                scanf("%ld", &fechaDevolucion);
                if (!esFechaValida(fechaDevolucion)) {
                    printf("[ERROR] Fecha invalida. Reintente.\n");
                } else if (fechaDevolucion < alquiler.fechaInicio) {
                    printf("[ERROR] La fecha de devolucion no puede ser anterior a la fecha de inicio (%ld).\n", 
                           alquiler.fechaInicio);
                }
            } while (!esFechaValida(fechaDevolucion) || fechaDevolucion < alquiler.fechaInicio);
            fVehiculos = fopen("vehiculos.dat", "rb+");
            if (fVehiculos != NULL) {
                while (fread(&vehiculo, sizeof(struct Vehiculo), 1, fVehiculos) == 1) {
                    if (strcmp(vehiculo.patente, alquiler.patente) == 0) {
                        precioDiaVehiculo = vehiculo.precio_dia;
                        strcpy(vehiculo.estado, "Disponible");
                        fseek(fVehiculos, -sizeof(struct Vehiculo), SEEK_CUR);
                        fwrite(&vehiculo, sizeof(struct Vehiculo), 1, fVehiculos);
                        break;
                    }
                }
                fclose(fVehiculos);
            }
            int diasReales = 0;
            long fIni = alquiler.fechaInicio;
            long fDev = fechaDevolucion;
            if (fDev > fIni) {
                int anioD = fDev / 10000, mesD = (fDev % 10000) / 100, diaD = fDev % 100;
                int anioI = fIni / 10000, mesI = (fIni % 10000) / 100, diaI = fIni % 100;
                diasReales = (anioD - anioI) * 365 + (mesD - mesI) * 30 + (diaD - diaI);
            }
            if (diasReales > alquiler.cantDias) {
                diasAdicionales = diasReales - alquiler.cantDias;
                recargoExtra = diasAdicionales * precioDiaVehiculo;
                alquiler.importeTotal += recargoExtra;
                printf("\n[AVISO DE EXCEDENTE] Se detectaron %d dia(s) de demora.\n", diasAdicionales);
                printf("Recargo automatico aplicado: $%.2f (Tarifa/dia: $%.2f)\n", recargoExtra, precioDiaVehiculo);
            } else {
                printf("\nVehiculo devuelto dentro del plazo acordado o sin recargo.\n");
            }
            alquiler.fechaEfectiva = fechaDevolucion;
            strcpy(alquiler.estado, "Finalizado");
            fseek(fAlquileres, -sizeof(struct Alquiler), SEEK_CUR);
            fwrite(&alquiler, sizeof(struct Alquiler), 1, fAlquileres);
            printf("\n==================================================\n");
            printf(">>> DEVOLUCION REGISTRADA EXITOSAMENTE!\n");
            printf("Importe Final Ajustado:  $%.2f\n", alquiler.importeTotal);
            printf("Saldo Pendiente de Pago: $%.2f\n", alquiler.importeTotal - alquiler.importePagado);
            printf("Estado del Vehiculo:     DISPONIBLE\n");
            printf("==================================================\n");
            break;
        }
    }
    if (!encontrado) {
        printf("[ERROR] No se encontro el alquiler Numero %d.\n", numAlquilerBuscado);
    }
    fclose(fAlquileres);
}

void listarAlquileres() {
    FILE *archivo;
    struct Alquiler alquiler;
    int cont = 0;
    system("cls");
    printf("\n--- LISTADO GENERAL DE ALQUILERES ---\n");
    archivo = fopen("alquileres.dat", "rb");
    if (archivo == NULL) {
        printf("[ERROR] No hay alquileres registrados en el sistema.\n");
        return;
    }
    printf("%-6s %-10s %-10s %-10s %-6s %-12s %-12s %-10s\n", 
           "NUM", "DNI", "PATENTE", "F. INICIO", "DIAS", "IMP. TOTAL", "IMP. PAGADO", "ESTADO");
    printf("----------------------------------------------------------------------------------------\n");
    while (fread(&alquiler, sizeof(struct Alquiler), 1, archivo) == 1) {
        printf("%-6d %-10ld %-10s %-10ld %-6d $%-11.2f $%-11.2f %-10s\n",
               alquiler.numAlquiler,
               alquiler.DNI,
               alquiler.patente,
               alquiler.fechaInicio,
               alquiler.cantDias,
               alquiler.importeTotal,
               alquiler.importePagado,
               alquiler.estado);
        cont++;
    }
    if (cont == 0) {
        printf("El archivo de alquileres esta vacio.\n");
    } else {
        printf("\nTotal de alquileres registrados: %d\n", cont);
    }
    fclose(archivo);
}
void modificarAlquiler() {
    FILE *fAlquileres, *fVehiculos;
    struct Alquiler alquiler;
    struct Vehiculo vehiculoViejo, vehiculoNuevo;
    int numAlquilerBuscado;
    int encontrado = 0;
    int patenteCambio = 0;
    char nuevaPatente[15];
    float precioDiaVehiculo = 0.0;
    system("cls");
    printf("\n--- MODIFICAR ALQUILER REGISTRADO ---\n");
    do {
        printf("Ingrese el Numero de Alquiler a modificar: ");
        if (scanf("%d", &numAlquilerBuscado) != 1) {
            printf("[ERROR] Debe ingresar un numero entero valido. Reintente.\n");
            while (getchar() != '\n');
            numAlquilerBuscado = 0;
        }
    } while (numAlquilerBuscado <= 0);
    fAlquileres = fopen("alquileres.dat", "rb+");
    if (fAlquileres == NULL) {
        printf("[ERROR] No se pudo abrir el archivo alquileres.dat o no hay registros.\n");
        return;
    }
    while (fread(&alquiler, sizeof(struct Alquiler), 1, fAlquileres) == 1) {
        if (alquiler.numAlquiler == numAlquilerBuscado) {
            encontrado = 1;
            if (strcmp(alquiler.estado, "Activo") != 0) {
                printf("\n[ERROR] No se puede modificar el alquiler Nro %d porque se encuentra %s.\n", 
                       numAlquilerBuscado, alquiler.estado);
                fclose(fAlquileres);
                return;
            }
            printf("\nAlquiler Encontrado:\n");
            printf("DNI Cliente: %ld | Patente Actual: %s | F. Inicio: %ld | Dias: %d | Total: $%.2f\n",
                   alquiler.DNI, alquiler.patente, alquiler.fechaInicio, alquiler.cantDias, alquiler.importeTotal);
            fflush(stdin);
            printf("\n¿Desea cambiar la patente del vehiculo? (S/N): ");
            char resp = getchar();
            while (getchar() != '\n');
            if (resp == 'S' || resp == 's') {
                printf("Ingrese la NUEVA patente del vehiculo: ");
                fgets(nuevaPatente, sizeof(nuevaPatente), stdin);
                nuevaPatente[strcspn(nuevaPatente, "\n")] = '\0';
                if (strcmp(nuevaPatente, alquiler.patente) != 0) {
                    fVehiculos = fopen("vehiculos.dat", "rb+");
                    if (fVehiculos == NULL) {
                        printf("[ERROR] No se pudo abrir vehiculos.dat.\n");
                        fclose(fAlquileres);
                        return;
                    }
                    int nuevoValido = 0;
                    while (fread(&vehiculoNuevo, sizeof(struct Vehiculo), 1, fVehiculos) == 1) {
                        if (strcmp(vehiculoNuevo.patente, nuevaPatente) == 0) {
                            if (strcmp(vehiculoNuevo.estado, "Disponible") == 0) {
                                nuevoValido = 1;
                                precioDiaVehiculo = vehiculoNuevo.precio_dia;
                            } else {
                                printf("[ERROR] El nuevo vehiculo (%s) no esta Disponible (Estado: %s).\n", 
                                       nuevaPatente, vehiculoNuevo.estado);
                            }
                            break;
                        }
                    }
                    if (!nuevoValido) {
                        printf("[ERROR] No se pudo realizar el cambio de vehiculo. Operacion cancelada.\n");
                        fclose(fVehiculos);
                        fclose(fAlquileres);
                        return;
                    }
                    strcpy(vehiculoNuevo.estado, "Alquilado");
                    fseek(fVehiculos, -sizeof(struct Vehiculo), SEEK_CUR);
                    fwrite(&vehiculoNuevo, sizeof(struct Vehiculo), 1, fVehiculos);
                    fseek(fVehiculos, 0, SEEK_SET);
                    while (fread(&vehiculoViejo, sizeof(struct Vehiculo), 1, fVehiculos) == 1) {
                        if (strcmp(vehiculoViejo.patente, alquiler.patente) == 0) {
                            strcpy(vehiculoViejo.estado, "Disponible");
                            fseek(fVehiculos, -sizeof(struct Vehiculo), SEEK_CUR);
                            fwrite(&vehiculoViejo, sizeof(struct Vehiculo), 1, fVehiculos);
                            break;
                        }
                    }
                    fclose(fVehiculos);
                    strcpy(alquiler.patente, nuevaPatente);
                    patenteCambio = 1;
                }
            }
            if (!patenteCambio) {
                fVehiculos = fopen("vehiculos.dat", "rb");
                if (fVehiculos != NULL) {
                    while (fread(&vehiculoViejo, sizeof(struct Vehiculo), 1, fVehiculos) == 1) {
                        if (strcmp(vehiculoViejo.patente, alquiler.patente) == 0) {
                            precioDiaVehiculo = vehiculoViejo.precio_dia;
                            break;
                        }
                    }
                    fclose(fVehiculos);
                }
            }
            do {
                printf("Nueva Fecha de inicio (AAAAMMDD): ");
                scanf("%ld", &alquiler.fechaInicio);
                if (!esFechaValida(alquiler.fechaInicio)) {
                    printf("[ERROR] Fecha invalida. Reintente.\n");
                }
            } while (!esFechaValida(alquiler.fechaInicio));
            do {
                printf("Nueva cantidad de dias contratados (> 0): ");
                scanf("%d", &alquiler.cantDias);
                if (alquiler.cantDias <= 0) {
                    printf("[ERROR] La cantidad de dias debe ser mayor a cero.\n");
                }
            } while (alquiler.cantDias <= 0);
            alquiler.importeTotal = alquiler.cantDias * precioDiaVehiculo;
            alquiler.fechaPrevista = alquiler.fechaInicio;
            fseek(fAlquileres, -sizeof(struct Alquiler), SEEK_CUR);
            fwrite(&alquiler, sizeof(struct Alquiler), 1, fAlquileres);

            printf("\n==================================================\n");
            printf(">>> ALQUILER MODIFICADO EXITOSAMENTE!\n");
            printf("Nro. de Alquiler:     %d\n", alquiler.numAlquiler);
            printf("Patente Asignada:     %s\n", alquiler.patente);
            printf("Nuevo Importe Total:  $%.2f\n", alquiler.importeTotal);
            printf("==================================================\n");
            break;
        }
    }

    if (!encontrado) {
        printf("[ERROR] No se encontro el alquiler Numero %d.\n", numAlquilerBuscado);
    }

    fclose(fAlquileres);
}