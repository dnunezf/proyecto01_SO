/*
* PROYECTO PROGRAMADO 01
* SISTEMAS OPERATIVOS
* ESTUDIANTE:
* David Núñez Franco, 119080008
* CREATED 13/04/2025
* */

#ifndef CONFIG_H
#define CONFIG_H

extern double mean_arrival_time[2];
extern double min_speed[2], max_speed[2];
extern double ambulance_probability;
extern int semaforo_time[2];
extern int k_values[2];
extern int bridge_length;

void leer_configuracion(const char* filename);

#endif //CONFIG_H
