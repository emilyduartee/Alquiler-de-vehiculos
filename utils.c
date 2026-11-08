#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "fechas.h"
#include "comun.h"

void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void limpiarPantalla(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausarPantalla(void) {
    printf("\nPresione ENTER para continuar...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void descartarRestoLinea(const char *buf) {
    if (strchr(buf, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

static void recortarEspacios(char *s) {
    int i = 0, j = 0;
    while (s[i] && isspace((unsigned char)s[i])) {
        i++;
    }
    while (s[i]) {
        s[j++] = s[i++];
    }
    s[j] = '\0';
    while (j > 0 && isspace((unsigned char)s[j - 1])) {
        s[--j] = '\0';
    }
}

int leerCadena(const char *msg, char *dest, int tam, int permitirVacio) {
    char buf[512];
    while (1) {
        if (msg && msg[0] != '\0') {
            printf("%s", msg);
            fflush(stdout);
        }
        if (!fgets(buf, sizeof(buf), stdin)) {
            return 0;
        }
        descartarRestoLinea(buf);
        buf[strcspn(buf, "\r\n")] = '\0';
        recortarEspacios(buf);

        if (!permitirVacio && strlen(buf) == 0) {
            printf("[ERROR] El dato ingresado no puede estar vacio. Reintente.\n");
            continue;
        }
        strncpy(dest, buf, (size_t)(tam - 1));
        dest[tam - 1] = '\0';
        return 1;
    }
}

int leerEntero(const char *msg, int min, int max, int *out) {
    char buf[64];
    while (1) {
        if (msg && msg[0] != '\0') {
            printf("%s", msg);
            fflush(stdout);
        }
        if (!fgets(buf, sizeof(buf), stdin)) {
            return 0;
        }
        descartarRestoLinea(buf);
        buf[strcspn(buf, "\r\n")] = '\0';
        recortarEspacios(buf);

        if (strlen(buf) == 0) {
            printf("[ERROR] Debe ingresar un valor numerico. Reintente.\n");
            continue;
        }

        char *endptr = NULL;
        long val = strtol(buf, &endptr, 10);
        if (*endptr != '\0') {
            printf("[ERROR] Debe ingresar solo numeros enteros. Reintente.\n");
            continue;
        }
        if (val < (long)min || val > (long)max) {
            printf("[ERROR] El valor debe estar entre %d y %d. Reintente.\n", min, max);
            continue;
        }

        *out = (int)val;
        return 1;
    }
}

int leerDouble(const char *msg, double minExclusivo, double *out) {
    char buf[64];
    while (1) {
        if (msg && msg[0] != '\0') {
            printf("%s", msg);
            fflush(stdout);
        }
        if (!fgets(buf, sizeof(buf), stdin)) {
            return 0;
        }
        descartarRestoLinea(buf);
        buf[strcspn(buf, "\r\n")] = '\0';
        recortarEspacios(buf);

        if (strlen(buf) == 0) {
            printf("[ERROR] Debe ingresar un importe numerico. Reintente.\n");
            continue;
        }

        char *endptr = NULL;
        double val = strtod(buf, &endptr);
        if (*endptr != '\0') {
            printf("[ERROR] Debe ingresar un importe numerico valido. Reintente.\n");
            continue;
        }
        if (val <= minExclusivo) {
            printf("[ERROR] El importe debe ser mayor a %.2f. Reintente.\n", minExclusivo);
            continue;
        }

        *out = val;
        return 1;
    }
}

int leerDNI(const char *msg, int *dni) {
    return leerEntero(msg, 1, 99999999, dni);
}

int leerPatente(const char *msg, char *dest) {
    char buf[64];
    while (1) {
        if (msg && msg[0] != '\0') {
            printf("%s", msg);
            fflush(stdout);
        }
        if (!fgets(buf, sizeof(buf), stdin)) {
            return 0;
        }
        descartarRestoLinea(buf);
        buf[strcspn(buf, "\r\n")] = '\0';
        recortarEspacios(buf);

        /* Remover espacios intermedios y convertir a mayusculas */
        char limpia[LON_PATENTE];
        int j = 0;
        int invalida = 0;
        for (int i = 0; buf[i] != '\0'; i++) {
            if (isspace((unsigned char)buf[i])) {
                continue;
            }
            if (!isalnum((unsigned char)buf[i])) {
                invalida = 1;
                break;
            }
            if (j < LON_PATENTE - 1) {
                limpia[j++] = (char)toupper((unsigned char)buf[i]);
            }
        }
        limpia[j] = '\0';

        if (invalida || j < 6 || j > 7) {
            printf("[ERROR] La patente debe tener 6 o 7 caracteres alfanumericos. Reintente.\n");
            continue;
        }

        strcpy(dest, limpia);
        return 1;
    }
}

int leerFecha(const char *msg, int *fecha) {
    int f = 0;
    while (1) {
        if (!leerEntero(msg, 19000101, 20991231, &f)) {
            return 0;
        }
        if (!esFechaValida(f)) {
            printf("[ERROR] Fecha invalida o incoherente (formato AAAAMMDD). Reintente.\n");
            continue;
        }
        *fecha = f;
        return 1;
    }
}

int leerSN(const char *msg) {
    char buf[64];
    while (1) {
        if (msg && msg[0] != '\0') {
            printf("%s", msg);
            fflush(stdout);
        }
        if (!fgets(buf, sizeof(buf), stdin)) {
            return 0;
        }
        descartarRestoLinea(buf);
        buf[strcspn(buf, "\r\n")] = '\0';
        recortarEspacios(buf);

        if (strlen(buf) == 1) {
            char c = (char)toupper((unsigned char)buf[0]);
            if (c == 'S') {
                return 1;
            }
            if (c == 'N') {
                return 0;
            }
        }
        printf("[ERROR] Ingrese 'S' para confirmar o 'N' para cancelar. Reintente.\n");
    }
}
