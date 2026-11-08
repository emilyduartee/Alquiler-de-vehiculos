#ifndef COMUN_H
#define COMUN_H

/* =========================================================================
   SISTEMA DE GESTION DE ALQUILER DE VEHICULOS
   Constantes de configuracion, estados y dimensiones de campos
   ========================================================================= */

/* Estados de Vehiculo */
#define EST_VEH_DISPONIBLE   "Disponible"
#define EST_VEH_ALQUILADO    "Alquilado"
#define EST_VEH_BAJA         "Baja"
#define EST_VEH_MANT         "En mantenimiento"

/* Estados de Alquiler */
#define EST_ALQ_ACTIVO       "Activo"
#define EST_ALQ_FINALIZADO   "Finalizado"
#define EST_ALQ_CANCELADO    "Cancelado"

/* Tipos de Movimiento */
#define MOV_PAGO             "Pago"
#define MOV_AJUSTE           "Ajuste"

/* Estados de Cliente */
#define CLI_ACTIVO           1
#define CLI_INACTIVO         0

/* Medios de Pago */
#define MEDIO_EFECTIVO       "Efectivo"
#define MEDIO_TARJETA        "Tarjeta"
#define MEDIO_TRANSFERENCIA  "Transferencia"
#define MEDIO_NINGUNO        "-"

/* Longitudes maximas de cadenas (incluyendo byte nulo) */
#define LON_PATENTE          15
#define LON_ESTADO           20
#define LON_TIPO_MOV         10
#define LON_MEDIO_PAGO       20
#define LON_NOMBRE           50
#define LON_TEL              20
#define LON_EMAIL            50
#define LON_DIR              50
#define LON_MARCA            30
#define LON_MODELO           30
#define LON_TIPO_VEH         30

/* Tolerancia para comparaciones de punto flotante */
#define TOLERANCIA_SALDO     0.005

/* Nombres de archivos de datos binarios */
#define ARCH_CLIENTES        "clientes.dat"
#define ARCH_VEHICULOS       "vehiculos.dat"
#define ARCH_ALQUILERES      "alquileres.dat"
#define ARCH_MOVIMIENTOS     "movimientos.dat"

#endif
