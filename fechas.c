#include <stdio.h>
#include <time.h>
#include "fechas.h"

int esBisiesto(int anio) {
    if ((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0)) {
        return 1;
    }
    return 0;
}

int diasEnMes(int anio, int mes) {
    if (mes < 1 || mes > 12) {
        return 0;
    }
    if (mes == 2) {
        return esBisiesto(anio) ? 29 : 28;
    }
    if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
        return 30;
    }
    return 31;
}

int esFechaValida(int f) {
    if (f <= 0) {
        return 0;
    }
    int anio = f / 10000;
    int mes = (f % 10000) / 100;
    int dia = f % 100;

    int anioActual = fechaActual() / 10000;
    int anioMax = anioActual + 5;

    if (anio < 1900 || anio > anioMax) {
        return 0;
    }
    if (mes < 1 || mes > 12) {
        return 0;
    }
    if (dia < 1 || dia > diasEnMes(anio, mes)) {
        return 0;
    }
    return 1;
}

int fechaADias(int f) {
    int y = f / 10000;
    int m = (f % 10000) / 100;
    int d = f % 100;

    y -= (m <= 2);
    int era = (y >= 0 ? y : y - 399) / 400;
    unsigned yoe = (unsigned)(y - era * 400);
    unsigned doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + (unsigned)d - 1;
    unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + (int)doe - 719468;
}

int diasAFecha(int n) {
    n += 719468;
    int era = (n >= 0 ? n : n - 146096) / 146097;
    unsigned doe = (unsigned)(n - era * 146097);
    unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int y = (int)yoe + era * 400;
    unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    unsigned mp = (5 * doy + 2) / 153;
    unsigned d = doy - (153 * mp + 2) / 5 + 1;
    unsigned m = mp < 10 ? mp + 3 : mp - 9;
    int finalYear = y + (m <= 2);
    return finalYear * 10000 + (int)m * 100 + (int)d;
}

int diasEntre(int f1, int f2) {
    return fechaADias(f2) - fechaADias(f1);
}

int sumarDias(int f, int n) {
    return diasAFecha(fechaADias(f) + n);
}

int fechaActual(void) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    if (!tm_info) {
        return 20261030;
    }
    return (tm_info->tm_year + 1900) * 10000 + (tm_info->tm_mon + 1) * 100 + tm_info->tm_mday;
}
