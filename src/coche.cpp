#include "coche.h"
#include "objLoader.h" 

Coche::Coche() {
    
    x = 0.3f;
    y = 0.0f;
    z = 0.0f;
    vx = vy = vz = 0.0f;
    ax = ay = az = 0.0f;
    psi = 90.0f;
    v_angular = 0.0f;
    angulo_ruedas = 0.0f;   
    rotacion_ruedas = 0.0f; 

    // Create a new model object
    model = new COBJModel();
    model_rueda = new COBJModel();
    //lectura objetos
    char ruta[] = "../x64/Release/OBJFiles/Car_metalic/coche_mod.obj";
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
    // He puesto valores aproximados para un coche deportivo. Ajustaremos luego.
    btCollisionShape* chassisShape = new btBoxShape(btVector3(1.1f, 4.0f, 0.5f));

    //CONFIGURACIÓN DE MASA E INERCIA
    btScalar masa = 800.0f; // 800 Kg
    btVector3 inerciaLocal(0, 0, 0);
    chassisShape->calculateLocalInertia(masa, inerciaLocal);

    //POSICIÓN INICIAL
    // Usamos la posición donde tienes el coche en el render (-80, -50, 295)
    // Le sumo un poco en Z (+2) para que caiga sobre el suelo y no lo atraviese al nacer.
    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(btVector3(-80.0f, -50.0f, 297.0f));

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

    // AÑADIR LAS RUEDAS
    // Necesitamos saber dónde van respecto al centro del coche.
    // Usaremos valores aproximados basados en tus glm::translate del render.

    float halfWidth = 2.f; // Antes era 0.9f (más estrecho)
    float connectionHeight = -0.3f; // Antes -0.2f. Al subir esto, el rayo nace más arriba.
    float wheelRadius = 0.6f;
    
    float frontLen = 0.5f; // Distancia al eje delantero
    float backLen = -6.7f; // Distancia al eje trasero
    // Direcciones para Z-UP
    btVector3 wheelDirectionCS0(0, 0, -1); // La rueda apunta hacia abajo (en Z negativo)
    btVector3 wheelAxleCS(-1, 0, 0);       // El eje de la rueda es X (izquierda/derecha)

    // Posición relativa al centro del coche (X, Y, Z)
    // --- Rueda Delantera Izquierda (0) ---
    btVector3 connectionPointCS0(halfWidth, frontLen, connectionHeight);
    m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, 0.6f, wheelRadius, m_tuning, true);

    // --- Rueda Delantera Derecha (1) ---
    btVector3 connectionPointCS1(-halfWidth, frontLen, connectionHeight);
    m_vehicle->addWheel(connectionPointCS1, wheelDirectionCS0, wheelAxleCS, 0.6f, wheelRadius, m_tuning, true);

    // --- Rueda Trasera Izquierda (2) ---
    btVector3 connectionPointCS2(halfWidth, backLen, connectionHeight);
    m_vehicle->addWheel(connectionPointCS2, wheelDirectionCS0, wheelAxleCS, 0.6f, wheelRadius, m_tuning, false);

    // --- Rueda Trasera Derecha (3) ---
    btVector3 connectionPointCS3(-halfWidth, backLen, connectionHeight);
    m_vehicle->addWheel(connectionPointCS3, wheelDirectionCS0, wheelAxleCS, 0.6f, wheelRadius, m_tuning, false);

    // 7. CONFIGURACIÓN DE SUSPENSIÓN (MÁS DURA)
    for (int i = 0; i < m_vehicle->getNumWheels(); i++) {
        btWheelInfo& wheel = m_vehicle->getWheelInfo(i);

        wheel.m_suspensionStiffness = 30.0f; // Antes 20. Más duro para que no se hunda.
        wheel.m_wheelsDampingRelaxation = 5.f;
        wheel.m_wheelsDampingCompression = 6.f;
        wheel.m_frictionSlip = 1000.0f;
        wheel.m_rollInfluence = 0.1f;

        // IMPORTANTE: Longitud de la suspensión
        // Si el rayo es muy corto, no toca el suelo. Dale margen.
        wheel.m_maxSuspensionTravelCm = 500.0f;
        wheel.m_suspensionRestLength1 = 0.6f; // Longitud del muelle en reposo
    }
}

#include <GLFW/glfw3.h> // Necesario para leer teclas
extern GLFWwindow* window; // Asumiendo que window es global en main o accesible

void Coche::update() {
    if (!m_vehicle) return;

    float engineForce = 0.0f;
    float steering = 0.0f;

    // Controles básicos (W/S acelera, A/D gira)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) engineForce = -4000.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) engineForce = 2000.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) steering = 0.5f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) steering = -0.5f;

    // Aplicar al motor (Tracción trasera: ruedas 2 y 3)
    m_vehicle->applyEngineForce(engineForce, 2);
    m_vehicle->applyEngineForce(engineForce, 3);

    // Aplicar dirección (Ruedas delanteras: 0 y 1)
    m_vehicle->setSteeringValue(steering, 0);
    m_vehicle->setSteeringValue(steering, 1);
}

void Coche::render(GLuint sh_programID, glm::mat4 MatriuVista) {
    if (!model) return;

    glm::mat4 ModelMatrix = glm::mat4(1.0f);
    float escala = 0.8f;

    // ===========================================================
    // PARTE NUEVA: OBTENER POSICIÓN DE BULLET
    // ===========================================================
    if (m_chassisBody && m_chassisBody->getMotionState()) {
        btTransform trans;
        // Le pedimos a Bullet la posición interpolada (suave)
        m_chassisBody->getMotionState()->getWorldTransform(trans);

        // Convertimos la matriz de Bullet a OpenGL
        float mat[16];
        trans.getOpenGLMatrix(mat); // Vuelca la posición y rotación en el array

        // Creamos la matriz de modelo a partir de la física
        ModelMatrix = glm::make_mat4(mat);
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(-90.0f), glm::vec3(0, 0, 1));
        // Bullet usa el centro de masa. Si tu modelo 3D no está centrado en (0,0,0),
        // quizás necesites un ajuste aquí. Por ahora lo probamos tal cual.
    }
    else {
        // Si no hay físicas, usamos el método antiguo (fallback)
        // PERO OJO: Quitamos las líneas que forzaban x,y,z fijos
        glm::mat4 TransMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
        ModelMatrix = TransMatrix;
    }
    // ===========================================================

    // Aplicamos el escalado (el coche visual suele ser más grande/pequeño que el físico)
    glm::mat4 ModelMatrixCar = glm::scale(ModelMatrix, glm::vec3(escala));

    // Calculamos la matriz de normales
    glm::mat4 NormalMatrixCar = glm::transpose(glm::inverse(MatriuVista * ModelMatrixCar));

    // Enviamos al Shader
    glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrixCar[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrixCar[0][0]);

    // Dibujamos la carrocería
    model->draw_TriVAO_OBJ(sh_programID);

    // DIBUJAR LAS RUEDAS (Sincronizadas con Bullet)
    if (model_rueda && m_vehicle) {
        for (int i = 0; i < m_vehicle->getNumWheels(); i++) {
            // Bullet actualiza la posición de la rueda automáticamente
            m_vehicle->updateWheelTransform(i, true);

            // Obtenemos la matriz de la rueda
            btTransform wheelTrans = m_vehicle->getWheelInfo(i).m_worldTransform;
            float matWheel[16];
            wheelTrans.getOpenGLMatrix(matWheel);

            glm::mat4 WheelMatrix = glm::make_mat4(matWheel);
            WheelMatrix = glm::scale(WheelMatrix, glm::vec3(escala));

            float ajuste = -0.3f;
            WheelMatrix = glm::translate(WheelMatrix, glm::vec3(ajuste, 0.0f, 0.0f));

            WheelMatrix = glm::rotate(WheelMatrix, glm::radians(90.0f), glm::vec3(0, 0, 1));

            glm::mat4 NormalMatrixWheel = glm::transpose(glm::inverse(MatriuVista * WheelMatrix));

            glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &WheelMatrix[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrixWheel[0][0]);

            model_rueda->draw_TriVAO_OBJ(sh_programID);
        }
    }
}
