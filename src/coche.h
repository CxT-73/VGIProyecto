#pragma once

#include "stdafx.h" 
#include <btBulletDynamicsCommon.h> 

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
    bool activadoABS;
    
    Coche();              // Constructor
    ~Coche();             // Destructor to free memory
    void initFisicas(btDiscreteDynamicsWorld* mundo);
    void update();
    void render(GLuint sh_programID, glm::mat4 MatriuVista);
    glm::mat4 getModelMatrixCar(float escala = 0.8f) const;

private:
    COBJModel* model;     // apuntador al modelo de la carroceria
    COBJModel* model_rueda;      // Un solo modelo para todas las ruedas
    //VARIABLES DE BULLET
    btRigidBody* m_chassisBody;           //cuerpo rígido del coche
    btRaycastVehicle* m_vehicle;          //controlador del vehículo
    btVehicleRaycaster* m_vehicleRayCaster; //Calcula los rayos de las ruedas al suelo
    btRaycastVehicle::btVehicleTuning m_tuning; //Configuración (suspensión, fricción, etc)
   
};
 