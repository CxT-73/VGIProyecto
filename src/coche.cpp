#include "coche.h"
#include "objLoader.h" 
#include <iostream>
extern ControlLuces controlLlumsCotxe;

Coche::Coche() {

    x = -80;
    y = -50;
    z = 295;
    velocidad_mph = 0.0f;
    ax = ay = az = 0.0f;
    psi = 90.0f;
    v_angular = 0.0f;
    angulo_ruedas = 0.0f;
    rotacion_ruedas = 0.0f;
    activadoABS = true;
    FrenoDeMano = true;
    // Create a new model object
    model = new COBJModel();
    model_rueda = new COBJModel();
    //lectura objetos
    char ruta[] = "../x64/Release/OBJFiles/Car/coche_mod.obj";
    if (model->LoadModel(ruta) != 0) {
        //mirem si carrega bé
        fprintf(stderr, "ERROR: Could not load car model from %s!\n", ruta);
    }
    else {
        printf("S'HA CARREGAT CORRECTAMENT: % s!\n", ruta);
    }
    char ruta_rueda[] = "../x64/Release/OBJFiles/Wheel/Rueda.obj";
    if (model_rueda->LoadModel(ruta_rueda) != 0) {
        fprintf(stderr, "ERROR: No se pudo cargar el modelo de la rueda desde %s!\n", ruta_rueda);

    }
    else {
        printf("RUEDA CARGADA CORRECTAMENTE: %s!\n", ruta_rueda);
    }
}

Coche::~Coche() {

    delete model;
    model = nullptr;
    delete model_rueda;
    model_rueda = nullptr;
}

// Función para inicializar las físicas del coche
void Coche::initFisicas(btDiscreteDynamicsWorld* mundo) {

    //FORMA DEL COCHE , EL CHASSIS
    btCollisionShape* chassisShape = new btBoxShape(btVector3(2.f, 6.1f, 0.5f));

    //CONFIGURACIÓN DE MASA E INERCIA
    btScalar masa = 1800.0f; // 800 Kg
    btVector3 inerciaLocal(0, 0, 0);
    chassisShape->calculateLocalInertia(masa, inerciaLocal);

    //POSICIÓN INICIAL
    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(btVector3(-502.5f, 985.0f, 304.8f));
     
    btQuaternion rotacion;
    rotacion.setRotation(btVector3(0, 0, -1), btScalar(2.00713f)); // 110 grados en Z
    tr.setRotation(rotacion);

    //CREAR EL cuerpo físico
    btDefaultMotionState* motionState = new btDefaultMotionState(tr);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(masa, motionState, chassisShape, inerciaLocal);
    m_chassisBody = new btRigidBody(rbInfo);

    // Evita que el coche se "duerma" si se queda quieto
    m_chassisBody->setActivationState(DISABLE_DEACTIVATION);

    //Añadimos el cuerpo al mundo
    mundo->addRigidBody(m_chassisBody);

    // CREAR EL VEHÍCULO 

    m_vehicleRayCaster = new btDefaultVehicleRaycaster(mundo);
    m_vehicle = new btRaycastVehicle(m_tuning, m_chassisBody, m_vehicleRayCaster);

    //Definir el sistema de coordenadas (Right, Up, Forward)
    // Como tu Z es vertical: 
    // Eje 0 = X (Derecha)
    // Eje 2 = Z (Arriba)
    // Eje 1 = Y (Adelante)
    m_vehicle->setCoordinateSystem(0, 2, 1);

    mundo->addVehicle(m_vehicle);

    btVector3 wheelDirectionCS0(0, 0, -1); // La rueda apunta hacia abajo (en Z negativo)
    btVector3 wheelAxleCS(-1, 0, 0);       // El eje de la rueda es X (izquierda/derecha)
    float wheelRadius = 0.9f;

    // X = Ancho 
    // Y = Largo 
    // Z = Altura 

    // Rueda 0: Delantera Izquierda
    btVector3 posDI(1.7f, 4.05f, 0.1f);

    // Rueda 1: Delantera Derecha
    btVector3 posDD(-1.7f, 4.05f, 0.1f);

    // Rueda 2: Trasera Izquierda
    btVector3 posTI(1.7f, -3.3f, 0.0f);

    // Rueda 3: Trasera Derecha
    btVector3 posTD(-1.7f, -3.3f, 0.0f);



    //Delantera Izq
    m_vehicle->addWheel(posDI, wheelDirectionCS0, wheelAxleCS, 0.6f, wheelRadius, m_tuning, true);

    //Delantera Der
    m_vehicle->addWheel(posDD, wheelDirectionCS0, wheelAxleCS, 0.6f, wheelRadius, m_tuning, true);

    //Trasera Izq
    m_vehicle->addWheel(posTI, wheelDirectionCS0, wheelAxleCS, 0.6f, wheelRadius, m_tuning, false);

    //Trasera Der
    m_vehicle->addWheel(posTD, wheelDirectionCS0, wheelAxleCS, 0.6f, wheelRadius, m_tuning, false);

    //CONFIGURACIÓN DE SUSPENSIÓN
    for (int i = 0; i < m_vehicle->getNumWheels(); i++) {
        btWheelInfo& wheel = m_vehicle->getWheelInfo(i);

        // AMORTIGUACIÓN (DAMPING)
        // Aumentamos la compresión para que no se hunda tan rápido el morro
        wheel.m_wheelsDampingRelaxation = 20.0f;
        wheel.m_wheelsDampingCompression = 30.0f; // Antes 15.0f -> Ahora 30.0f (Resiste el hundimiento)

        //AGARRE Y SEGURIDAD
        wheel.m_frictionSlip = 3.0f;
        wheel.m_rollInfluence = 0.1f; // Pequeño truco: 0.1 ayuda a que no vuelque tan fácil lateralmente

        //FUERZA MÁXIMA
        wheel.m_maxSuspensionForce = 40000.0f;
        wheel.m_maxSuspensionTravelCm = 500.0f;

        // RIGIDEZ (STIFFNESS)
        if (i == 2 || i == 3) { // TRASERAS
            wheel.m_suspensionStiffness = 120.0f;
            wheel.m_suspensionRestLength1 = 0.7f;
        }
        else {
            // DELANTERAS (IMPORTANTE)
            // Subimos de 90 a 160 para que el morro no toque el suelo al frenar
            wheel.m_suspensionStiffness = 160.0f;
            wheel.m_suspensionRestLength1 = 0.65f;
        }
    }
}

#include <GLFW/glfw3.h> // Necesario para leer teclas
extern GLFWwindow* window; // Asumiendo que window es global en main o accesible

void Coche::update() {
    if (!m_vehicle) return;

    static bool kPressed = false;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        if (!kPressed) {
            activadoABS = !activadoABS;
            printf("Sistema ABS: %s\n", activadoABS ? "ACTIVADO" : "DESACTIVADO");
            kPressed = true;
        }
    }
    else {
        kPressed = false;
    }

    static bool pPressed = false;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (!pPressed) {
            FrenoDeMano = !FrenoDeMano;
            printf("Freno de Mano: %s\n", FrenoDeMano ? "PUESTO" : "QUITADO");
            pPressed = true;
        }
    }
    else {
        pPressed = false;
    }

    // DATOS
    float velocidadActual = m_vehicle->getCurrentSpeedKmHour();
    float fuerzaMotor = 0.0f;
    float fuerzaFreno = 0.0f;

    // CONFIGURACIÓN DE FUERZAS
    float potAcelerar = -5000.0f; // Fuerza para ir adelante
    float potAtras = 4000.0f;     // Fuerza para ir atrás

    // VALORES AUMENTADOS PARA QUE FRENE DE VERDAD
    float frenoABS = 300.0f;
    float frenoBloqueo = 15000.0f;
    float frenoParking = 100000.0f;

    // INPUTS
    bool W = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
    bool S = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
    bool A = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
    bool D = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
    bool Espacio = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS); // Freno
    bool P = (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS);

    if (FrenoDeMano) {
        fuerzaMotor = 0.0f;
        fuerzaFreno = frenoParking;
    }
    else {
        if (W) {
            fuerzaMotor = potAcelerar;
        }
        else if (S) {
            fuerzaMotor = potAtras;
        }

        if (Espacio) {
            if (activadoABS) {
                fuerzaFreno = frenoABS;
            }
            else {
                fuerzaFreno = frenoBloqueo;
            }
        }
    }

    // DIRECCIÓN
    float incremento = 0.01f;
    float recuperacion = 0.05f;
    float maxGiro = 0.5f;

    if (A) {
        angulo_ruedas += incremento;
        if (angulo_ruedas > maxGiro) angulo_ruedas = maxGiro;
    }
    else if (D) {
        angulo_ruedas -= incremento;
        if (angulo_ruedas < -maxGiro) angulo_ruedas = -maxGiro;
    }
    else {
        if (angulo_ruedas > 0.0f) {
            angulo_ruedas -= recuperacion;
            if (angulo_ruedas < 0.0f) angulo_ruedas = 0.0f;
        }
        else if (angulo_ruedas < 0.0f) {
            angulo_ruedas += recuperacion;
            if (angulo_ruedas > 0.0f) angulo_ruedas = 0.0f;
        }
    }

    if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {

        int axisCount;
        const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);
        int buttonCount;
        const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

        if (std::abs(axes[0]) > 0.1f) {
            angulo_ruedas = axes[0] * -0.5f;
        }

        if (!FrenoDeMano) {
            float valorR2 = axes[4];
            float valorL2 = axes[3];
            float deadzoneTriggers = 0.05f;

            if (valorR2 > deadzoneTriggers) {
                fuerzaMotor = potAcelerar * valorR2;
            }
            else if (valorL2 > deadzoneTriggers) {
                fuerzaMotor = potAtras * valorL2;
            }

            if (buttons[0] == GLFW_PRESS) {
                Espacio = true; // Actualizamos para lógica interna
                if (activadoABS) fuerzaFreno = frenoABS;
                else fuerzaFreno = frenoBloqueo;
            }

            if (valorL2 > deadzoneTriggers || buttons[0] == GLFW_PRESS) {
                controlLlumsCotxe.frenando = true;
            }
            else {
                controlLlumsCotxe.frenando = false;
            }
        }
    }

    // PRIORIDAD DE FRENO: Si frenamos, cortamos motor
    if (fuerzaFreno > 0.0f) {
        fuerzaMotor = 0.0f;
    }

    // APLICAR FUERZAS AL MOTOR
    m_vehicle->applyEngineForce(fuerzaMotor, 2);
    m_vehicle->applyEngineForce(fuerzaMotor, 3);
    m_vehicle->applyEngineForce(0, 0);
    m_vehicle->applyEngineForce(0, 1);

    // APLICAR FRENOS
    if (fuerzaFreno > 0.0f) {
        if (FrenoDeMano) {
            m_vehicle->setBrake(fuerzaFreno, 0);
            m_vehicle->setBrake(fuerzaFreno, 1);
            m_vehicle->setBrake(fuerzaFreno, 2);
            m_vehicle->setBrake(fuerzaFreno, 3);
        }
        else if (activadoABS) {
            // Reparto 60% Delante / 40% Detrás para frenada estable
            m_vehicle->setBrake(fuerzaFreno * 0.6f, 0);
            m_vehicle->setBrake(fuerzaFreno * 0.6f, 1);
            m_vehicle->setBrake(fuerzaFreno * 0.4f, 2);
            m_vehicle->setBrake(fuerzaFreno * 0.4f, 3);
        }
        else {
            // Frenada bloqueo (solo traseras para derrape o todas según prefieras)
            // Aquí pongo bloqueo trasero para facilitar derrape controlado
            m_vehicle->setBrake(0.0f, 0);
            m_vehicle->setBrake(0.0f, 1);
            m_vehicle->setBrake(fuerzaFreno, 2);
            m_vehicle->setBrake(fuerzaFreno, 3);
        }
    }
    else {
        // Liberar frenos
        m_vehicle->setBrake(0, 0);
        m_vehicle->setBrake(0, 1);
        m_vehicle->setBrake(0, 2);
        m_vehicle->setBrake(0, 3);
    }

    // Dirección con penalización si bloqueamos ruedas (solo sin ABS)
    float anguloFinal = angulo_ruedas;
    if (!activadoABS && Espacio && std::abs(velocidadActual) > 1.0f && !FrenoDeMano) {
        anguloFinal *= 0.05f; // El volante apenas responde bloqueando
    }

    m_vehicle->setSteeringValue(anguloFinal, 0);
    m_vehicle->setSteeringValue(anguloFinal, 1);
}

void Coche::render(GLuint sh_programID, glm::mat4 MatriuVista) {
    if (!model) return;

    glm::mat4 ModelMatrix = glm::mat4(1.0f);
    float escala = 0.8f;

    if (m_chassisBody && m_chassisBody->getMotionState()) {
        btTransform trans;
        // Le pedimos a Bullet la posición interpolada (suave)
        m_chassisBody->getMotionState()->getWorldTransform(trans);

        // Obtener el vector de origen (posición)
        btVector3 bulletPos = trans.getOrigin();

        //Convertir a glm::vec3
        x = bulletPos.getX();
        y = bulletPos.getY();
        z = bulletPos.getZ();

        btVector3 velocidadVector = m_chassisBody->getLinearVelocity();
        float velocidadMetrosSegundo = velocidadVector.length();
        velocidad_mph = velocidadMetrosSegundo * 2.23694f;

        btMatrix3x3 basis = trans.getBasis();
        btVector3 bulletForward = basis.getColumn(1);
        float radians = atan2(bulletForward.getX(), bulletForward.getY());
        float degrees = glm::degrees(radians);
        psi = degrees;

        // Convertimos la matriz de Bullet a OpenGL
        float mat[16];
        trans.getOpenGLMatrix(mat); // Vuelca la posición y rotación en el array

        // Creamos la matriz de modelo a partir de la física
        ModelMatrix = glm::make_mat4(mat);
    }
    else {
        glm::mat4 TransMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
        ModelMatrix = TransMatrix;
    }

    // Aplicamos el escalado (el coche visual suele ser más grande/pequeño que el físico)
    glm::mat4 ModelMatrixCar = glm::scale(ModelMatrix, glm::vec3(escala));

    // Calculamos la matriz de normales
    glm::mat4 NormalMatrixCar = glm::transpose(glm::inverse(MatriuVista * ModelMatrixCar));

    // Enviamos al Shader
    glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrixCar[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrixCar[0][0]);

    // Dibujamos la carrocería
    model->draw_TriVAO_OBJ(sh_programID);

    // DIBUJAR LAS RUEDAS 
    if (model_rueda && m_vehicle) {
        for (int i = 0; i < m_vehicle->getNumWheels(); i++) {
            // Bullet actualiza la posición de la rueda automáticamente
            m_vehicle->updateWheelTransform(i, true);

            // Obtenemos la matriz de la rueda
            btTransform wheelTrans = m_vehicle->getWheelInfo(i).m_worldTransform;
            float matWheel[16];
            wheelTrans.getOpenGLMatrix(matWheel);

            glm::mat4 WheelMatrix = glm::make_mat4(matWheel);
            float escala_ruedas = 1.f;
            WheelMatrix = glm::scale(WheelMatrix, glm::vec3(escala_ruedas));

            WheelMatrix = glm::rotate(WheelMatrix, glm::radians(90.0f), glm::vec3(0, 0, 1));

            glm::mat4 NormalMatrixWheel = glm::transpose(glm::inverse(MatriuVista * WheelMatrix));

            glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &WheelMatrix[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrixWheel[0][0]);

            model_rueda->draw_TriVAO_OBJ(sh_programID);
        }
    }
}

glm::mat4 Coche::getModelMatrixCar(float escala) const {
    glm::mat4 M(1.0f);
    if (m_chassisBody && m_chassisBody->getMotionState()) {
        btTransform trans;
        m_chassisBody->getMotionState()->getWorldTransform(trans);
        float mat[16];
        trans.getOpenGLMatrix(mat);
        M = glm::make_mat4(mat);
    }
    else {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
        glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(psi), glm::vec3(0, 0, 1));
        M = T * R;
    }
    M = glm::scale(M, glm::vec3(escala));
    return M;
}

void Coche::reiniciarPosicion() {
    if (!m_chassisBody || !m_vehicle) return;

    //DETENER EL COCHE
    m_chassisBody->setLinearVelocity(btVector3(0, 0, 0));
    m_chassisBody->setAngularVelocity(btVector3(0, 0, 0));

    //PREPARAR LA POSICIÓN Y ROTACIÓN INICIAL
    btTransform tr;
    tr.setIdentity();

    // Posición original (La misma que en initFisicas)
    tr.setOrigin(btVector3(-502.5f, 985.0f, 304.8f));

    // Rotación original
    btQuaternion rotacion;
    rotacion.setRotation(btVector3(0, 0, -1), btScalar(2.00713f));
    tr.setRotation(rotacion);

    //APLICAR EL "TELETRANSPORTE" AL CUERPO FÍSICO
    m_chassisBody->setCenterOfMassTransform(tr);

    if (m_chassisBody->getMotionState()) {
        m_chassisBody->getMotionState()->setWorldTransform(tr);
    }

    angulo_ruedas = 0.0f;    
    m_vehicle->setSteeringValue(0.0f, 0);
    m_vehicle->setSteeringValue(0.0f, 1);

    FrenoDeMano = true;

}