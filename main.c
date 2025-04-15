/*
* PROYECTO PROGRAMADO 01
* SISTEMAS OPERATIVOS
* ESTUDIANTE:
* David Núñez Franco, 119080008
* CREATED 13/04/2025
* */

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "carnage_mode.h"
#include "semaforo_mode.h"
#include "oficiales_mode.h"

int main() {
    leer_configuracion("config.txt");

    int modo;
    printf("Seleccione el modo de administración del puente:\n");
    printf("1. Carnage (FIFO por dirección)\n");
    printf("2. Semáforo\n");
    printf("3. Oficiales de tránsito\n");
    printf("Modo: ");
    scanf("%d", &modo);

    switch (modo) {
        case 1:
            printf("Iniciando modo Carnage...\n");
            iniciar_carnage();
        break;

        case 2:
            printf("Iniciando modo Semáforo...\n");
            iniciar_semaforo();
        break;

        case 3:
            printf("Iniciando modo Oficiales de Tránsito...\n");
            iniciar_oficiales();
        break;

        default:
            printf("Modo inválido.\n");
        break;
    }

    return 0;
}