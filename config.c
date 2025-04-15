/*
* PROYECTO PROGRAMADO 01
* SISTEMAS OPERATIVOS
* ESTUDIANTE:
* David Núñez Franco, 119080008
* CREATED 13/04/2025
* */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

// SE DEFINEN LOS ARREGLOS GLOBALES PARA PARAMETROS DE CONFIGURACION POR DIRECCION
double mean_arrival_time[2];
double min_speed[2], max_speed[2];
double ambulance_probability;

// TIEMPOS DE SEMAFORO PARA CADA DIRECCION (SECONDS)
int semaforo_time[2] = {5, 5};

// CANTIDAD DE VEHICULOS PERMITIDOS POR OFICIALES
int k_values[2] = {3, 2};

// LEE PARAMETROS DE SIMULACION DESDE UN ARCHIVO DE CONFIGURACION. VALIDO CLAVE=VALOR
void leer_configuracion(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("No se pudo abrir el archivo de configuración");
        exit(1);
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        char key[64];
        double value;

        // IGNORA LINEAS VACIAS O COMENTARIOS
        if (line[0] == '#' || sscanf(line, "%[^=]=%lf", key, &value) != 2) continue;

        // ASOCIA CADA CLAVE LEIDA CON SU VARIABLE GLOBAL
        if (strcmp(key, "mean_arrival_time_east") == 0) mean_arrival_time[0] = value;
        else if (strcmp(key, "mean_arrival_time_west") == 0) mean_arrival_time[1] = value;
        else if (strcmp(key, "min_speed_east") == 0) min_speed[0] = value;
        else if (strcmp(key, "max_speed_east") == 0) max_speed[0] = value;
        else if (strcmp(key, "min_speed_west") == 0) min_speed[1] = value;
        else if (strcmp(key, "max_speed_west") == 0) max_speed[1] = value;
        else if (strcmp(key, "ambulance_probability") == 0) ambulance_probability = value;
        else if (strcmp(key, "semaforo_time_east") == 0) semaforo_time[0] = (int)value;
        else if (strcmp(key, "semaforo_time_west") == 0) semaforo_time[1] = (int)value;
        else if (strcmp(key, "k1") == 0) k_values[0] = (int)value;
        else if (strcmp(key, "k2") == 0) k_values[1] = (int)value;
    }

    fclose(file);
}