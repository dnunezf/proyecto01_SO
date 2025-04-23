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

// ESTRUCTURA DE UN VEHICULO / AMBULANCIA
typedef struct {
    int id;
    int direction;
    int is_ambulance;
    double speed;
} Vehicle;

// VARIABLES DE SINCRONIZACION DE MODO OFICIALES
pthread_mutex_t oficiales_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t oficiales_cond = PTHREAD_COND_INITIALIZER;

// VECTOR DE PUENTE
Vehicle** oficiales_puente;

int oficiales_puente_ocupado = 0;
int oficiales_vehicle_id = 0;
int turno_actual = EAST; // DIRECCION ACTUALMENTE HABILITADA
int k_counter = 0; // CANTIDAD DE VEHICULOS QUE HAN PASADO EN TURNO ACTUAL
int oficiales_ambulancia_en_espera[2] = {0, 0}; //AMBULANCIAS ESPERANDO POR DIRECCION

/*
 * HILO CONTROLADOR DE TRAFICO. CAMBIA DE DIRECCION SI SE CUMPLE EL MAX. K O SI NO HAY VEHICULOS
 * EN LA DIRECCION ACTUAL.
*/
void* oficiales_controller(void* arg) {
    while (1) {
        pthread_mutex_lock(&oficiales_mutex);

        // Cambiar de sentido cuando se cumpla K o no haya vehículos
        int max_k = (turno_actual == EAST) ? k_values[0] : k_values[1];

        if (k_counter >= max_k || (oficiales_ambulancia_en_espera[turno_actual] == 0 && oficiales_puente_ocupado == 0)) {
            turno_actual = 1 - turno_actual; // CAMBIA EL TURNO
            k_counter = 0;
            printf("👮 Cambio de dirección: ahora el turno es para %s\n", turno_actual == EAST ? "Este" : "Oeste");
            pthread_cond_broadcast(&oficiales_cond); // NOTIFICA A LOS VEHICULOS
        }

        pthread_mutex_unlock(&oficiales_mutex);
        sleep(1); // CHEQUEA PERIODICAMENTE
    }
}

/*
 * HILO QUE REPRESENTA UN VEHICULO INTENTANDO CRUZAR EL PUENTE.
 * SOLO CRUZA SI ES SU TURNO O SI ES AMBULANCIA.
*/
void* oficiales_vehicle_thread(void* arg) {
    Vehicle* v = (Vehicle*) arg;

    pthread_mutex_lock(&oficiales_mutex);
    if (v->is_ambulance) oficiales_ambulancia_en_espera[v->direction]++;

    // ESPERA SU TURNO, ESPACIO O SEA AMBULANCIA
    while (((v->direction != turno_actual && !v->is_ambulance) || (oficiales_puente_ocupado >= bridge_length))) {
        pthread_cond_wait(&oficiales_cond, &oficiales_mutex);
    }

    if (v->is_ambulance) oficiales_ambulancia_en_espera[v->direction]--;

    // INGRESA AL PUENTE
    oficiales_puente[oficiales_puente_ocupado++] = v;
    k_counter++;

    if (v->is_ambulance)
        printf("🚑 Ambulancia %d entra desde %s (con prioridad)\n", v->id, v->direction == EAST ? "Este" : "Oeste");
    else
        printf("🚗 Vehículo %d entra desde %s\n", v->id, v->direction == EAST ? "Este" : "Oeste");

    pthread_mutex_unlock(&oficiales_mutex);

    // SIMULA EL CRUCE DEL PUENTE
    sleep((int)(100.0 / v->speed));

    pthread_mutex_lock(&oficiales_mutex);

    oficiales_puente_ocupado--;
    printf("🚙 Vehículo %d salió del puente\n", v->id);

    pthread_cond_broadcast(&oficiales_cond); // NOTIFICA QUE PUEDE HABER ESPACIO PARA OTRO
    pthread_mutex_unlock(&oficiales_mutex);

    free(v);
    return NULL;
}

/*
 * GENERADOR DE VEHICULOS PARA UNA DIRECCION ESPECIFICA. CREA VEHICULOS CON VELOCIDAD
 * Y TIPO ALEATORIO (AUTO / AMBULANCIA)
*/
void* oficiales_generar_vehiculos(void* dir_ptr) {
    int direction = *(int*)dir_ptr;
    while (1) {
        Vehicle* v = malloc(sizeof(Vehicle));
        v->id = __sync_fetch_and_add(&oficiales_vehicle_id, 1);
        v->direction = direction;
        v->is_ambulance = ((double) rand() / RAND_MAX) < ambulance_probability;
        v->speed = random_speed(direction);

        pthread_t tid;
        pthread_create(&tid, NULL, oficiales_vehicle_thread, v);
        pthread_detach(tid); // NO SE ESPERA SU FINALIZACION

        sleep((int)random_exponential(mean_arrival_time[direction]));
    }
    return NULL;
}

/*
 * INICIA LA SIMULACION DEL MODO OFICIALES, LANZANDO EL HILO CONTROLADOR Y GENERADORES DE VEHICULOS
*/
void iniciar_oficiales() {
    srand(time(NULL));

    // INICIALIZA EL VECTOR DEL PUENTE CON TAMAÑO DINAMICO
    oficiales_puente = malloc(sizeof(Vehicle*) * bridge_length);

    pthread_t controller_tid, east_gen, west_gen;

    int d0 = EAST, d1 = WEST;

    pthread_create(&controller_tid, NULL, oficiales_controller, NULL);
    pthread_create(&east_gen, NULL, oficiales_generar_vehiculos, &d0);
    pthread_create(&west_gen, NULL, oficiales_generar_vehiculos, &d1);

    pthread_join(controller_tid, NULL);
    pthread_join(east_gen, NULL);
    pthread_join(west_gen, NULL);

    free(oficiales_puente);
}