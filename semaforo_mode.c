/*
* PROYECTO PROGRAMADO 01
* SISTEMAS OPERATIVOS
* ESTUDIANTE:
* David Núñez Franco, 119080008
* CREATED 13/04/2025
* */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "config.h"
#include "common.h"

#define EAST 0
#define WEST 1

// REPRESENTACION DE UN VEHICULO / AMBULANCIA
typedef struct {
    int id;
    int direction;
    int is_ambulance;
    double speed;
} Vehicle;

// VARIABLES DE SINCRONIZACION PARA EL MODO SEMAFORO
pthread_mutex_t semaforo_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t semaforo_cond = PTHREAD_COND_INITIALIZER;

// ESTADO DEL PUENTE EN MODO SEMAFORO
int semaforo_bridge_count = 0;
int current_green = EAST; // DIRECCION QUE TIENE LUZ VERDE
int semaforo_vehicle_id_counter = 0;
int semaforo_ambulancia_en_espera[2] = {0, 0};

/*
 * HILO QUE CONTROLA EL CAMBIO DE LUCES DE SEMAFORO, ALTERNANDO ENTRE ESTE Y OESTE, CADA N SEGUNDOS
*/
void* semaforo_controller(void* arg) {
    while (1) {
        sleep(semaforo_time[current_green]); // ESPERA EL TIEMPO ASIGNADO

        pthread_mutex_lock(&semaforo_mutex);
        current_green = 1 - current_green; // CAMBIO DE DIRECCIONES
        printf("🔁 Cambio de luz: ahora el verde está en el %s\n", current_green == EAST ? "Este" : "Oeste");
        pthread_cond_broadcast(&semaforo_cond); // NOTIFICA A TODOS LOS HILOS
        pthread_mutex_unlock(&semaforo_mutex);
    }
}

/*
 * HILO INDIVIDUAL POR VEHICULO QUE INTENTA CRUZAR EL PUENTE. SOLO PUEDE PASAR SI TIENE LUZ VERDE O AMBULANCIA
*/
void* semaforo_vehicle_thread(void* arg) {
    Vehicle* v = (Vehicle*) arg;

    pthread_mutex_lock(&semaforo_mutex);
    if (v->is_ambulance) semaforo_ambulancia_en_espera[v->direction]++;

    // ESPERA LUZ VERDE O PRIORIDAD POR SER AMBULANCIA
    while ((v->direction != current_green && !v->is_ambulance) ||
           (semaforo_bridge_count > 0 && semaforo_bridge_count < 0)) {
        pthread_cond_wait(&semaforo_cond, &semaforo_mutex);
    }

    if (v->is_ambulance) semaforo_ambulancia_en_espera[v->direction]--;

    semaforo_bridge_count++;

    if (v->is_ambulance)
        printf("🚑 Ambulancia %d entra desde %s (con prioridad)\n", v->id, v->direction == EAST ? "Este" : "Oeste");
    else
        printf("🚗 Vehículo %d entra desde %s\n", v->id, v->direction == EAST ? "Este" : "Oeste");

    pthread_mutex_unlock(&semaforo_mutex);

    // SIMULA EL CRUCE DEL PUENTE
    sleep((int)(100.0 / v->speed));

    pthread_mutex_lock(&semaforo_mutex);
    semaforo_bridge_count--;

    printf("🚙 Vehículo %d salió del puente\n", v->id);

    pthread_cond_broadcast(&semaforo_cond);
    pthread_mutex_unlock(&semaforo_mutex);

    free(v);
    return NULL;
}

/*
 * GENERADOR DE VEHICULOS PARA UNA DIRECCION ESPECIFICA. CREA HILOS CON TIEMPOS DE LLEGADA SEGUN
 * DISTRIBUCION EXPONENCIAL.
*/
void* semaforo_generar_vehiculos(void* dir_ptr) {
    int direction = *(int*)dir_ptr;

    while (1) {
        Vehicle* v = malloc(sizeof(Vehicle));
        v->id = __sync_fetch_and_add(&semaforo_vehicle_id_counter, 1);
        v->direction = direction;
        v->is_ambulance = ((double) rand() / RAND_MAX) < ambulance_probability;
        v->speed = random_speed(direction);

        pthread_t tid;
        pthread_create(&tid, NULL, semaforo_vehicle_thread, v);
        pthread_detach(tid); // NO SE ESPERA SU FINALIZACION

        sleep((int)random_exponential(mean_arrival_time[direction]));
    }
    return NULL;
}

/*
 * INICIA LA SIMULACION DEL MODO SEMAFORO, LANZANDO EL HILO CONTROLADOR Y GENERADORES DE VEHICULOS
*/
void iniciar_semaforo() {
    srand(time(NULL));
    pthread_t semaforo_tid, east_gen, west_gen;

    int d0 = EAST;
    int d1 = WEST;

    pthread_create(&semaforo_tid, NULL, semaforo_controller, NULL);
    pthread_create(&east_gen, NULL, semaforo_generar_vehiculos, &d0);
    pthread_create(&west_gen, NULL, semaforo_generar_vehiculos, &d1);

    pthread_join(semaforo_tid, NULL);
    pthread_join(east_gen, NULL);
    pthread_join(west_gen, NULL);
}