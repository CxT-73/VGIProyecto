#pragma once

#include "stdafx.h" 


class COBJModel;

class Coche {
public:
   
    float x, y, z;        // Posición
    float vx, vy, vz;     // Velocidad
    float ax, ay, az;     // Aceleracion
    float psi;            // posicion angular (hacia donde apunta el coche. 0 es delante.)
    float v_angular;      // velocidad angular
    float angulo_ruedas;   // ángulo de giro
    float rotacion_ruedas; // rotación por movimiento (gira mas rapido contra haya mas velocidad)

    
    Coche();              // Constructor
    ~Coche();             // Destructor to free memory
    void render(GLuint sh_programID, glm::mat4 MatriuVista);

private:
    COBJModel* model;     // apuntador al modelo de la carroceria
    COBJModel* model_rueda;      // Un solo modelo para todas las ruedas
};