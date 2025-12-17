#pragma once

#include "stdafx.h" 
#include <btBulletDynamicsCommon.h> 

class COBJModel;

class Coche {
public:
   
    float x, y, z;        // Posición
    float velocidad_mph;  //velocidad
    float ax, ay, az;     // Aceleracion
    float psi;            // posicion angular (hacia donde apunta el coche. 0 es delante.)
    float v_angular;      // velocidad angular
    float angulo_ruedas;   // ángulo de giro
    float rotacion_ruedas; // rotación por movimiento (gira mas rapido contra haya mas velocidad)
    bool activadoABS;
    bool FrenoDeMano;
    bool sonidoAceleracionActivo = false;
    Coche();              // Constructor
    ~Coche();             // Destructor to free memory
    void initFisicas(btDiscreteDynamicsWorld* mundo);
    void update();
    void reiniciarPosicion();
    void render(GLuint sh_programID, glm::mat4 MatriuVista);
    glm::mat4 getModelMatrixCar(float escala = 0.8f) const;
    btRigidBody* getChassisBody() const { return m_chassisBody; }
    float getVelocidad() { return velocidad_mph; }
    btVector3 getLinearVelocity() {return m_chassisBody->getLinearVelocity();}
    btVector3 getAngularVelocity() { return m_chassisBody->getAngularVelocity(); }
    void setLinearVelocity(const btVector3& vel) {
        if (m_chassisBody) {
            m_chassisBody->setLinearVelocity(vel);
            m_chassisBody->activate(true); // Importante: Despierta el cuerpo si estaba dormido
        }
    }

    void setAngularVelocity(const btVector3& vel) {
        if (m_chassisBody) {
            m_chassisBody->setAngularVelocity(vel);
            m_chassisBody->activate(true); // Importante: Despierta el cuerpo
        }
    }
	btRigidBody* getRigidBody() const { return m_chassisBody; }

private:
    COBJModel* model;     // apuntador al modelo de la carroceria
    COBJModel* model_rueda;      // Un solo modelo para todas las ruedas
    //VARIABLES DE BULLET
    btRigidBody* m_chassisBody;           //cuerpo rígido del coche
    btRaycastVehicle* m_vehicle;          //controlador del vehículo
    btVehicleRaycaster* m_vehicleRayCaster; //Calcula los rayos de las ruedas al suelo
    btRaycastVehicle::btVehicleTuning m_tuning; //Configuración (suspensión, fricción, etc)
   
};
 