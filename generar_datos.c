#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comun.h"
#include "clientes.h"
#include "vehiculos.h"
#include "alquileres.h"
#include "movimientos.h"

static struct Cliente clientesData[10] = {
    {30111222, "Gomez Martin", "3764-100001", "mgomez@mail.com", "Av. Mitre 123", 20240110, CLI_ACTIVO},
    {31222333, "Fernandez Lucia", "3764-100002", "lfernandez@mail.com", "Colon 456", 20240215, CLI_ACTIVO},
    {32333444, "Rodriguez Pablo", "3764-100003", "prodriguez@mail.com", "San Martin 789", 20240320, CLI_ACTIVO},
    {33444555, "Lopez Carolina", "3764-100004", "clopez@mail.com", "Sarmiento 321", 20240505, CLI_ACTIVO},
    {34555666, "Martinez Diego", "3764-100005", "dmartinez@mail.com", "Belgrano 654", 20240612, CLI_ACTIVO},
    {35666777, "Sosa Valeria", "3764-100006", "vsosa@mail.com", "Rivadavia 987", 20240801, CLI_ACTIVO},
    {36777888, "Benitez Javier", "3764-100007", "jbenitez@mail.com", "Junin 159", 20240910, CLI_ACTIVO},
    {37888999, "Acosta Romina", "3764-100008", "racosta@mail.com", "Cordoba 753", 20241120, CLI_ACTIVO},
    {38999000, "Duarte Nicolas", "3764-100009", "nduarte@mail.com", "Entre Rios 852", 20250118, CLI_ACTIVO},
    {39000111, "Romero Agustina", "3764-100010", "aromero@mail.com", "Salta 951", 20250302, CLI_INACTIVO}
};

static struct Vehiculo vehiculosData[10] = {
    {"AA111AA", "Toyota", "Corolla", 2022, "Auto", 5, 45000.0, EST_VEH_ALQUILADO},
    {"AB222BB", "Ford", "Ranger", 2021, "Camioneta", 5, 70000.0, EST_VEH_DISPONIBLE},
    {"AC333CC", "Volkswagen", "Gol", 2020, "Auto", 5, 30000.0, EST_VEH_DISPONIBLE},
    {"AD444DD", "Renault", "Kangoo", 2019, "Utilitario", 3, 40000.0, EST_VEH_ALQUILADO},
    {"AE555EE", "Chevrolet", "Onix", 2023, "Auto", 5, 38000.0, EST_VEH_DISPONIBLE},
    {"AF666FF", "Fiat", "Cronos", 2022, "Auto", 5, 36000.0, EST_VEH_MANT},
    {"AG777GG", "Peugeot", "208", 2021, "Auto", 5, 34000.0, EST_VEH_BAJA},
    {"AH888HH", "Toyota", "Hilux", 2023, "Camioneta", 5, 85000.0, EST_VEH_ALQUILADO},
    {"AI999II", "Citroen", "Berlingo", 2020, "Utilitario", 3, 42000.0, EST_VEH_DISPONIBLE},
    {"AJ000JJ", "Volkswagen", "Amarok", 2022, "Camioneta", 5, 90000.0, EST_VEH_DISPONIBLE}
};

static struct Alquiler alquileresData[15] = {
    {1,  30111222, "AA111AA", 20260105, 20260110, 20260110,  5, 225000.0, 225000.0, EST_ALQ_FINALIZADO},
    {2,  31222333, "AB222BB", 20260112, 20260115, 20260115,  3, 210000.0, 210000.0, EST_ALQ_FINALIZADO},
    {3,  32333444, "AC333CC", 20260120, 20260127, 20260130,  7, 300000.0, 210000.0, EST_ALQ_FINALIZADO},
    {4,  33444555, "AE555EE", 20260201, 20260205, 20260205,  4, 152000.0, 152000.0, EST_ALQ_FINALIZADO},
    {5,  34555666, "AI999II", 20260210, 20260220, 20260220, 10, 420000.0, 200000.0, EST_ALQ_FINALIZADO},
    {6,  35666777, "AJ000JJ", 20260301, 20260303, 20260303,  2, 180000.0, 180000.0, EST_ALQ_FINALIZADO},
    {7,  30111222, "AB222BB", 20260315, 20260321, 20260322,  6, 490000.0, 490000.0, EST_ALQ_FINALIZADO},
    {8,  36777888, "AD444DD", 20260401, 20260406, 20260406,  5, 200000.0, 100000.0, EST_ALQ_FINALIZADO},
    {9,  37888999, "AA111AA", 20260415, 20260418,        0,  3,      0.0,      0.0, EST_ALQ_CANCELADO},
    {10, 38999000, "AC333CC", 20260502, 20260506,        0,  4,  50000.0,  50000.0, EST_ALQ_CANCELADO},
    {11, 31222333, "AE555EE", 20260510, 20260518, 20260518,  8, 304000.0, 304000.0, EST_ALQ_FINALIZADO},
    {12, 33444555, "AB222BB", 20260601, 20260606,        0,  5,      0.0,      0.0, EST_ALQ_CANCELADO},
    {13, 34555666, "AA111AA", 20261026, 20261102,        0,  7, 315000.0, 100000.0, EST_ALQ_ACTIVO},
    {14, 32333444, "AD444DD", 20261028, 20261102,        0,  5, 200000.0,      0.0, EST_ALQ_ACTIVO},
    {15, 35666777, "AH888HH", 20261020, 20261030,        0, 10, 850000.0, 850000.0, EST_ALQ_ACTIVO}
};

struct MovimientoAux {
    int numMov;
    int numAlq;
    const char *tipo;
    double monto;
    int fecha;
    const char *medio;
};

static struct MovimientoAux movsAux[23] = {
    {1,  1,  MOV_PAGO,   100000.0, 20260105, MEDIO_EFECTIVO},
    {2,  1,  MOV_PAGO,   125000.0, 20260110, MEDIO_TARJETA},
    {3,  2,  MOV_PAGO,   210000.0, 20260112, MEDIO_TRANSFERENCIA},
    {4,  3,  MOV_PAGO,   100000.0, 20260120, MEDIO_EFECTIVO},
    {5,  3,  MOV_PAGO,   110000.0, 20260127, MEDIO_TARJETA},
    {6,  3,  MOV_AJUSTE,  90000.0, 20260130, MEDIO_NINGUNO},
    {7,  4,  MOV_PAGO,   152000.0, 20260201, MEDIO_TARJETA},
    {8,  5,  MOV_PAGO,   100000.0, 20260210, MEDIO_EFECTIVO},
    {9,  5,  MOV_PAGO,   100000.0, 20260220, MEDIO_TRANSFERENCIA},
    {10, 6,  MOV_PAGO,   180000.0, 20260301, MEDIO_TARJETA},
    {11, 7,  MOV_PAGO,   300000.0, 20260315, MEDIO_EFECTIVO},
    {12, 7,  MOV_AJUSTE,  70000.0, 20260322, MEDIO_NINGUNO},
    {13, 7,  MOV_PAGO,   190000.0, 20260322, MEDIO_TRANSFERENCIA},
    {14, 8,  MOV_PAGO,   100000.0, 20260401, MEDIO_EFECTIVO},
    {15, 9,  MOV_AJUSTE,-135000.0, 20260416, MEDIO_NINGUNO},
    {16, 10, MOV_PAGO,    50000.0, 20260502, MEDIO_EFECTIVO},
    {17, 10, MOV_AJUSTE, -70000.0, 20260504, MEDIO_NINGUNO},
    {18, 11, MOV_PAGO,   150000.0, 20260510, MEDIO_TARJETA},
    {19, 11, MOV_PAGO,   154000.0, 20260518, MEDIO_EFECTIVO},
    {20, 12, MOV_AJUSTE,-350000.0, 20260602, MEDIO_NINGUNO},
    {21, 15, MOV_PAGO,   500000.0, 20261020, MEDIO_TRANSFERENCIA},
    {22, 15, MOV_PAGO,   350000.0, 20261025, MEDIO_TARJETA},
    {23, 13, MOV_PAGO,   100000.0, 20261026, MEDIO_EFECTIVO}
};

int main(void) {
    printf("Generando dataset oficial de prueba (informe seccion 11)...\n");

    /* 1. Clientes */
    FILE *fCli = fopen(ARCH_CLIENTES, "wb");
    if (!fCli) {
        perror("Error creando clientes.dat");
        return 1;
    }
    fwrite(clientesData, sizeof(struct Cliente), 10, fCli);
    fclose(fCli);
    printf("[OK] clientes.dat generado (%d registros, %u bytes).\n",
           10, (unsigned)(10 * sizeof(struct Cliente)));

    /* 2. Vehiculos */
    FILE *fVeh = fopen(ARCH_VEHICULOS, "wb");
    if (!fVeh) {
        perror("Error creando vehiculos.dat");
        return 1;
    }
    fwrite(vehiculosData, sizeof(struct Vehiculo), 10, fVeh);
    fclose(fVeh);
    printf("[OK] vehiculos.dat generado (%d registros, %u bytes).\n",
           10, (unsigned)(10 * sizeof(struct Vehiculo)));

    /* 3. Alquileres */
    FILE *fAlq = fopen(ARCH_ALQUILERES, "wb");
    if (!fAlq) {
        perror("Error creando alquileres.dat");
        return 1;
    }
    fwrite(alquileresData, sizeof(struct Alquiler), 15, fAlq);
    fclose(fAlq);
    printf("[OK] alquileres.dat generado (%d registros, %u bytes).\n",
           15, (unsigned)(15 * sizeof(struct Alquiler)));

    /* 4. Movimientos */
    struct Movimiento movimientosData[23];
    memset(movimientosData, 0, sizeof(movimientosData));

    for (int i = 0; i < 23; i++) {
        movimientosData[i].numMovimiento = movsAux[i].numMov;
        movimientosData[i].numAlquiler = movsAux[i].numAlq;
        strncpy(movimientosData[i].tipoMov, movsAux[i].tipo, LON_TIPO_MOV - 1);
        movimientosData[i].monto = movsAux[i].monto;
        movimientosData[i].fecha = movsAux[i].fecha;
        strncpy(movimientosData[i].medioPago, movsAux[i].medio, LON_MEDIO_PAGO - 1);

        /* Asociar DNI y Patente desde el alquiler correspondiente */
        int alqIndex = movsAux[i].numAlq - 1;
        movimientosData[i].DNI = alquileresData[alqIndex].DNI;
        strncpy(movimientosData[i].patente, alquileresData[alqIndex].patente, LON_PATENTE - 1);
    }

    FILE *fMov = fopen(ARCH_MOVIMIENTOS, "wb");
    if (!fMov) {
        perror("Error creando movimientos.dat");
        return 1;
    }
    fwrite(movimientosData, sizeof(struct Movimiento), 23, fMov);
    fclose(fMov);
    printf("[OK] movimientos.dat generado (%d registros, %u bytes).\n",
           23, (unsigned)(23 * sizeof(struct Movimiento)));

    /* 5. Archivos de texto individuales de movimientos por cliente */
    int dnisConMov[9] = {
        30111222, 31222333, 32333444, 33444555, 34555666,
        35666777, 36777888, 37888999, 38999000
    };

    for (int k = 0; k < 9; k++) {
        char nombreTxt[64];
        sprintf(nombreTxt, "movimientos_%d.txt", dnisConMov[k]);
        FILE *fTxt = fopen(nombreTxt, "w");
        if (!fTxt) {
            printf("Error creando %s\n", nombreTxt);
            continue;
        }
        for (int i = 0; i < 23; i++) {
            if (movimientosData[i].DNI == dnisConMov[k]) {
                /* Formato CSV ;: numMov;numAlq;Tipo;monto;fecha;DNI;patente;medio */
                fprintf(fTxt, "%d;%d;%s;%.2f;%d;%d;%s;%s\n",
                        movimientosData[i].numMovimiento,
                        movimientosData[i].numAlquiler,
                        movimientosData[i].tipoMov,
                        movimientosData[i].monto,
                        movimientosData[i].fecha,
                        movimientosData[i].DNI,
                        movimientosData[i].patente,
                        movimientosData[i].medioPago);
            }
        }
        fclose(fTxt);
        printf("[OK] %s generado.\n", nombreTxt);
    }

    printf("\nDATASET OFICIAL GENERADO EXITOSAMENTE!\n");
    return 0;
}
