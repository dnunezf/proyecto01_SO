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
#include <time.h>
#include "config.h"
#include "common.h"

#define EAST 0
#define WEST 1

// ESTRUCTURA QUE REPRESENTA UN VEHICULO / AMBULANCIA
typedef struct {
    int id;
    int direction;
    int is_ambulance;
    double speed;
} Vehicle;

// VARIABLES DE SINCRONIZACION
pthread_mutex_t carnage_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t carnage_cond = PTHREAD_COND_INITIALIZER;

// VECTOR DE PUENTE
Vehicle** carnage_puente;

int carnage_puente_ocupado = 0;
int bridge_direction = -1; // -1 ES LIBRE, 0 ES ESTE, 1 ES OESTE
int carnage_vehicle_id_counter = 0; // CONTADOR DE ID

/*
 * FUNCION EJECUTADA POR CADA VEHICULO (HILO). CONTROLA INGRESO AL PUENTE, EN ESQUEMA CARNAGE
*/
void* carnage_vehicle_thread(void* arg) {
    Vehicle* v = (Vehicle*) arg;

    pthread_mutex_lock(&carnage_mutex);

    // ESPERA HASTA QUE HAYA ESPACIO EN EL PUENTE Y DIRECCION SEA LA MISMA
    while ((carnage_puente_ocupado >= bridge_length) || (carnage_puente_ocupado > 0 && bridge_direction != v->direction)) {
        pthread_cond_wait(&carnage_cond, &carnage_mutex);
    }

    // INGRESA AL PUENTE
    carnage_puente[carnage_puente_ocupado++] = v;
    bridge_direction = v->direction;

    if (v->is_ambulance)
        printf("🚑 Ambulancia %d entra desde %s\n", v->id, v->direction == EAST ? "Este" : "Oeste");
    else
        printf("🚗 Vehículo %d entra desde %s\n", v->id, v->direction == EAST ? "Este" : "Oeste");

    pthread_mutex_unlock(&carnage_mutex);

    // SIMULA TIEMPO DE CRUCE SEGUN VELOCIDAD
    sleep((int)(100.0 / v->speed));

    pthread_mutex_lock(&carnage_mutex);

    // VEHICULO TERMINA DE CRUZAR
    carnage_puente_ocupado--;
    printf("🚙 Vehículo %d salió del puente\n", v->id);

    // SI NO HAY MAS VEHICULOS, SE LIBERA EL PUENTE
    if (carnage_puente_ocupado == 0)
        bridge_direction = -1;

    pthread_cond_broadcast(&carnage_cond); // NOTIFICA A OTROS HILOS EN ESPERA
    pthread_mutex_unlock(&carnage_mutex);

    free(v); // LIBERA MEMORIA DEL VEHICULO
    return NULL;
}

/*
 * FUNCION QUE GENERA VEHICULOS EN BUCLE. APLICA DISTRIBUCION EXPONENCIAL AL TIEMPO ENTRE LLEGADAS
*/
void* carnage_generar_vehiculos(void* dir_ptr) {
    int direction = *(int*)dir_ptr;
    while (1) {
        Vehicle* v = malloc(sizeof(Vehicle));
        v->id = __sync_fetch_and_add(&carnage_vehicle_id_counter, 1);
        v->direction = direction;

        if (direction == EAST)
            v->is_ambulance = ((double) rand() / RAND_MAX) < ambulance_probability_east;
        else
            v->is_ambulance = ((double) rand() / RAND_MAX) < ambulance_probability_west;

        v->speed = random_speed(direction);

        pthread_t tid;
        pthread_create(&tid, NULL, carnage_vehicle_thread, v);
        pthread_detach(tid); // NO SE ESPERA A QUE TERMINE

        sleep((int)random_exponential(mean_arrival_time[direction]));
    }
    return NULL;
}

/*
 * INICIALIZA LA SIMULACION DE MODO CARNAGE. CREA HILOS GENERADORES, PARA AMBAS DIRECCIONES
*/
void iniciar_carnage() {
    srand(time(NULL));

    // INICIALIZA EL VECTOR DEL PUENTE CON TAMAÑO DINAMICO
    carnage_puente = malloc(sizeof(Vehicle*) * bridge_length);

    pthread_t east_gen, west_gen;

    int d0 = EAST;
    int d1 = WEST;

    pthread_create(&east_gen, NULL, carnage_generar_vehiculos, &d0);
    pthread_create(&west_gen, NULL, carnage_generar_vehiculos, &d1);

    pthread_join(east_gen, NULL);
    pthread_join(west_gen, NULL);

    free(carnage_puente);
}