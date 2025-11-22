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
void Coche::render(GLuint sh_programID, glm::mat4 MatriuVista) {
    if (!model) return;

    float escala = 0.8f;
    x = -80.0f;
    y = -50.0f;
    z = 295.0f;

    glm::mat4 ModelMatrix = glm::mat4(1.0f);
     
    glm::mat4 TransMatrix = glm::translate(ModelMatrix, glm::vec3(x, y, z));
     
    glm::mat4 CarOrientationMatrix = TransMatrix;
     
    glm::mat4 ModelMatrixCar = glm::scale(CarOrientationMatrix, glm::vec3(escala));
    glm::mat4 NormalMatrixCar = glm::transpose(glm::inverse(MatriuVista * ModelMatrixCar));

    glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrixCar[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrixCar[0][0]);
    model->draw_TriVAO_OBJ(sh_programID);

    if (model_rueda) {
        // Posiciones relativas de las ruedas
        glm::vec3 pos_rueda_DI = glm::vec3(-0.76f, -0.4f, -0.15f);
        glm::vec3 pos_rueda_TI = glm::vec3(0.25f, -0.4f, -0.10f);
        glm::vec3 pos_rueda_DD = glm::vec3(-0.85f, -0.4f, 1.80f);
        glm::vec3 pos_rueda_TD = glm::vec3(0.14f, -0.4f, 1.85f);

        float movAtras = -9.0f;
        float movDer = 2.9f;
         
        glm::mat4 ModelMatrixCarRueda = glm::scale(CarOrientationMatrix, glm::vec3(1.0f));

        auto dibujarRueda = [&](glm::vec3 pos, glm::vec3 offsetLocal) {
            glm::mat4 ModelMatrixRueda = ModelMatrixCarRueda;
            ModelMatrixRueda = glm::translate(ModelMatrixRueda, pos + offsetLocal);

            glm::mat4 NormalMatrixRueda = glm::transpose(glm::inverse(MatriuVista * ModelMatrixRueda));
            glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrixRueda[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrixRueda[0][0]);
            model_rueda->draw_TriVAO_OBJ(sh_programID);
            };

        dibujarRueda(pos_rueda_DI, glm::vec3(0.4f, -1.8f, -0.8f));
        dibujarRueda(pos_rueda_DD, glm::vec3(0.3f, 1.8f, -2.8f));
        dibujarRueda(pos_rueda_TI, glm::vec3(6.8, -1.5f, -1.0f));
        dibujarRueda(pos_rueda_TD, glm::vec3(6.7f, 2.2f, -2.9f));
    }
}
