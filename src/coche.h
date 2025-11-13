#pragma once

#include "stdafx.h" 
#pragma once
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Vehicle/btRaycastVehicle.h>

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

    Coche(btDiscreteDynamicsWorld* world);  // nuevo: recibe el mundo físico
    ~Coche();

    void update(float dt);
    void render(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, CColor col_object, bool sw_mat[5]);

    void aplicarInput(float motor, float giro, float freno);

private:
    COBJModel* model;     // apuntador al modelo de la carroceria
    COBJModel* model_rueda;      // Un solo modelo para todas las ruedas
    btDiscreteDynamicsWorld* mundo;     // referencia al mundo físico
    btRigidBody* chasis;                // cuerpo principal
    btRaycastVehicle* vehiculo;         // sistema de ruedas
    btVehicleRaycaster* raycaster;      // raycaster para suspensión
};