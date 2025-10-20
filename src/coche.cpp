#include "coche.h"
#include "objLoader.h" 

Coche::Coche() {
    
    x = 0.3f;
    y = 0.0f;
    z = 0.0f;
    vx = vy = vz = 0.0f;
    ax = ay = az = 0.0f;
    psi = 0.0f;
    v_angular = 0.0f;
    angulo_ruedas = 0.0f;   
    rotacion_ruedas = 0.0f; 

    // Create a new model object
    model = new COBJModel();
    model_rueda = new COBJModel();
    //lectura objetos
    char ruta[] = "../x64/Release/OBJFiles/Car/coche_sr.obj";
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

void Coche::render(GLuint sh_programID, glm::mat4 MatriuVista) {
    if (!model) return; 
    // Apliquem transformacions
    glm::mat4 ModelMatrix = glm::mat4(1.0f); // Inicia matriz modelo (identidad).

    float scaleFactor = 10.5f;               // Factor de escala para la carrocería.
    ModelMatrix = glm::translate(ModelMatrix, glm::vec3(x, y, z)); // Mueve a la posición (x,y,z) del coche.

    // Aplica rotación 'psi' (yaw) alrededor del eje Y.
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(psi), glm::vec3(0.0f, 1.0f, 0.0f));

    // Guarda estado actual (posición + psi) para usarlo como base para las ruedas.
    glm::mat4 BaseCarMatrix = ModelMatrix;
    // Aplica rotación fija de -90 grados en Y (concreto de este modelo por como se mostraba al exportarlo al inicio).
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Aplica la escala final a la carrocería.
    ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaleFactor));

    // Calcula la matriz normal (para iluminación correcta con transformaciones).
    glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MatriuVista * ModelMatrix));

    //Send the matrices to the active shader
    glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);

    //Draw the model
    model->draw_TriVAO_OBJ(sh_programID);

    if (model_rueda) { 

        glm::vec3 pos_rueda_DI = glm::vec3(-0.76f, -0.4f, -0.15f); // Delantera Izquierda 
        glm::vec3 pos_rueda_DD = glm::vec3(0.25f, -0.4f, -0.10f); // Delantera Derecha 
        glm::vec3 pos_rueda_TI = glm::vec3(-0.85f, -0.4f, 1.80f); // Trasera Izquierda 
        glm::vec3 pos_rueda_TD = glm::vec3(0.14f, -0.4f, 1.85f); // Trasera Derecha  

        
        float scaleFactorRueda = 0.25f; 
        glm::vec3 escalaRueda = glm::vec3(scaleFactorRueda);

        // Rueda Delantera Izquierda (DI)
        glm::mat4 ModelMatrixRuedaDI = BaseCarMatrix; // Partimos de la matriz base del coche (antes de escalar y rotar coche)
        ModelMatrixRuedaDI = glm::translate(ModelMatrixRuedaDI, pos_rueda_DI);
   
        ModelMatrixRuedaDI = glm::scale(ModelMatrixRuedaDI, escalaRueda);
        glm::mat4 NormalMatrixRuedaDI = glm::transpose(glm::inverse(MatriuVista * ModelMatrixRuedaDI));
        glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrixRuedaDI[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrixRuedaDI[0][0]);
        model_rueda->draw_TriVAO_OBJ(sh_programID);

        // Rueda Delantera Derecha (DD)
        glm::mat4 ModelMatrixRuedaDD = BaseCarMatrix;
        ModelMatrixRuedaDD = glm::translate(ModelMatrixRuedaDD, pos_rueda_DD);

        ModelMatrixRuedaDD = glm::scale(ModelMatrixRuedaDD, escalaRueda);
        glm::mat4 NormalMatrixRuedaDD = glm::transpose(glm::inverse(MatriuVista * ModelMatrixRuedaDD));
        glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrixRuedaDD[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrixRuedaDD[0][0]);
        model_rueda->draw_TriVAO_OBJ(sh_programID);

        // Rueda Trasera Izquierda (TI)
        glm::mat4 ModelMatrixRuedaTI = BaseCarMatrix;
        ModelMatrixRuedaTI = glm::translate(ModelMatrixRuedaTI, pos_rueda_TI);

        ModelMatrixRuedaTI = glm::scale(ModelMatrixRuedaTI, escalaRueda);
        glm::mat4 NormalMatrixRuedaTI = glm::transpose(glm::inverse(MatriuVista * ModelMatrixRuedaTI));
        glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrixRuedaTI[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrixRuedaTI[0][0]);
        model_rueda->draw_TriVAO_OBJ(sh_programID);

        // Rueda Trasera Derecha (TD)
        glm::mat4 ModelMatrixRuedaTD = BaseCarMatrix;
        ModelMatrixRuedaTD = glm::translate(ModelMatrixRuedaTD, pos_rueda_TD);

        ModelMatrixRuedaTD = glm::scale(ModelMatrixRuedaTD, escalaRueda);
        glm::mat4 NormalMatrixRuedaTD = glm::transpose(glm::inverse(MatriuVista * ModelMatrixRuedaTD));
        glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrixRuedaTD[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrixRuedaTD[0][0]);
        model_rueda->draw_TriVAO_OBJ(sh_programID);
    }
}