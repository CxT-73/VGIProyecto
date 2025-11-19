#include "coche.h"
#include "objLoader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include "material.h"

char ruta_coche[] = "../x64/Release/OBJFiles/Car/coche_sr.obj";
char ruta_rueda[] = "../x64/Release/OBJFiles/Wheel/Rueda.obj";

Coche::Coche(btDiscreteDynamicsWorld* world) : mundo(world)
{
    // Cargar modelos visuales 
    model = new COBJModel();
    model->LoadModel(ruta_coche);

    model_rueda = new COBJModel();
    model_rueda->LoadModel(ruta_rueda);

    // Crear chasis físico 
    btCollisionShape* chassisShape = new btBoxShape(btVector3(1.0f, 0.5f, 2.0f));
    btScalar mass = 800.0f;
    btVector3 inertia(0, 0, 0);
    chassisShape->calculateLocalInertia(mass, inertia);

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(0, 0, 0));

    btDefaultMotionState* motion = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, chassisShape, inertia);
    chasis = new btRigidBody(info);
    chasis->setActivationState(DISABLE_DEACTIVATION);
    mundo->addRigidBody(chasis);

    //Eliminar una vez fixeado lo de abajo
    chasis->setActivationState(DISABLE_DEACTIVATION);
    chasis->setLinearVelocity(btVector3(0, 0, 0));
    chasis->setAngularVelocity(btVector3(0, 0, 0));
    // Desactivar gravedad temporalmente
    mundo->setGravity(btVector3(0, 0, -9.81f));

    // Crear sistema de vehículo 
    raycaster = new btDefaultVehicleRaycaster(mundo);
    btRaycastVehicle::btVehicleTuning tuning;
    vehiculo = new btRaycastVehicle(tuning, chasis, raycaster);
    mundo->addVehicle(vehiculo);

    // Añadir ruedas 
    btVector3 wheelDirection(0, 0, -1);   // hacia donde apunta la suspension
    btVector3 wheelAxle(-1, 0, 0);        // eje de rotación (izquierda-derecha)
    vehiculo->setCoordinateSystem(0, 1, 2);
    float suspensionRestLength = 0.3f;
    float wheelRadius = 0.4f;

    // --- Añadimos las 4 ruedas manualmente ---
    // Orden: (posición), dirección, eje, suspensión, radio, tuning, delantera/trasera

    // 1. Delantera derecha
    vehiculo->addWheel(
        btVector3(0.9f, 0.05f, 0.f),   // X=ancho, Y=frontal, Z=altura
        wheelDirection,
        wheelAxle,
        suspensionRestLength,
        wheelRadius,
        tuning,
        true);

//    // 2. Delantera izquierda
//    vehiculo->addWheel(
//        btVector3(-0.9f, -0.3f, 1.7f),
//        wheelDirection,
//        wheelAxle,
//        suspensionRestLength,
//        wheelRadius,
//        tuning,
//        true);
//
//    // 3. Trasera derecha
//    vehiculo->addWheel(
//        btVector3(0.9f, -0.3f, -1.7f),
//        wheelDirection,
//        wheelAxle,
//        suspensionRestLength,
//        wheelRadius,
//        tuning,
//        false);
//
//    // 4. Trasera izquierda
//    vehiculo->addWheel(
//        btVector3(-0.9f, -0.3f, -1.7f),
//        wheelDirection,
//        wheelAxle,
//        suspensionRestLength,
//        wheelRadius,
//        tuning,
//        false);
//}
//
//Coche::~Coche() {
//    mundo->removeRigidBody(chasis);
//    delete vehiculo;
//    delete raycaster;
//    delete chasis;
//    delete model;
//    delete model_rueda;
}

void Coche::update(float dt)
{
    mundo->stepSimulation(dt);

    btTransform trans;
    chasis->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    btScalar yaw, pitch, roll;
    trans.getBasis().getEulerYPR(yaw, pitch, roll);

    x = pos.getX();
    y = pos.getY();
    z = pos.getZ();
    psi = yaw;
}

//control (WASD o gamepad)
void Coche::aplicarInput(float motor, float giro, float freno)
{
    vehiculo->applyEngineForce(motor, 2); //rear wheels
    vehiculo->applyEngineForce(motor, 3);
    vehiculo->setBrake(freno, 2);
    vehiculo->setBrake(freno, 3);
    vehiculo->setSteeringValue(giro, 0);//delanteras
    vehiculo->setSteeringValue(giro, 1);
}

// Render visual usando COBJModel 
void Coche::render(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, CColor col_object, bool sw_mat[5])
{
    SeleccionaColorMaterial(sh_programID, col_object, sw_mat);

    // Matriz de modelo
    btTransform trans;
    chasis->getMotionState()->getWorldTransform(trans);
    btScalar m[16];
    trans.getOpenGLMatrix(m);
    glm::mat4 M = glm::make_mat4(m);

    // Ajustes de orientación y escala
    // Rotar el modelo para alinear sus ejes con los de Bullet.
    //Escalarlo para que coincida con el tamaño del chasis físico.
    //M = glm::rotate(M, glm::radians(90.0f), glm::vec3(1, 0, 0));
    M = glm::scale(M, glm::vec3(5.f));

    //Enviar matriz al shader
    glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &M[0][0]);
    model->draw_TriVAO_OBJ(sh_programID);


    // Dibujar ruedas
    for (int i = 0; i < vehiculo->getNumWheels(); i++)
    {
        btWheelInfo& wheel = vehiculo->getWheelInfo(i);
        btTransform t = wheel.m_worldTransform;
        btVector3 p = t.getOrigin();
        btQuaternion q = t.getRotation();

        glm::mat4 Mw = glm::translate(glm::mat4(1.0f), glm::vec3(p.getX(), p.getY(), p.getZ()));
        Mw *= glm::mat4_cast(glm::quat(q.getW(), q.getX(), q.getY(), q.getZ()));

        Mw = glm::rotate(Mw, glm::radians(90.f), glm::vec3(0, 1, 0)); 
        Mw = glm::scale(Mw, glm::vec3(0.1f));                          

        glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &Mw[0][0]);
        model_rueda->draw_TriVAO_OBJ(sh_programID);
    }
}