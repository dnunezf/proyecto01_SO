cmake_minimum_required(VERSION 3.10)
project(proyecto_puente C)

set(CMAKE_C_STANDARD 99)

add_executable(proyecto_puente
        main.c
        config.c
        common.c
        carnage_mode.c
        semaforo_mode.c
        oficiales_mode.c
)

target_link_libraries(proyecto_puente m)
