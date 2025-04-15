/*
* PROYECTO PROGRAMADO 01
* SISTEMAS OPERATIVOS
* ESTUDIANTE:
* David Núñez Franco, 119080008
* CREATED 13/04/2025
* */

#include <stdlib.h>
#include <math.h>
#include "config.h"

/*
 * GENERA UN VALOR ALEATORIO CON DIST. EXPONENCIAL, PARA MODELAR EL TIEMPO ENTRE LLEGADA
 * DE VEHICULOS.
 * Fórmula: -media * ln(1 - r), donde r es un aleatorio ∈ [0,1)
*/
double random_exponential(double mean) {
    double r = (double) rand() / RAND_MAX;
    return -mean * log(1 - r);
}

/*
 * GENERA UNA VELOCIDAD ALEATORIA UNIFORME DENTRO DEL RANGO PERMITIDO, PARA UNA DIRECCION ESPECIFICA.
 * (EAST = 0, WEST = 1).
 */
double random_speed(int direction) {
    double r = (double) rand() / RAND_MAX;
    return min_speed[direction] + r * (max_speed[direction] - min_speed[direction]);
}
