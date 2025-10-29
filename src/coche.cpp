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


void Coche::render(GLuint sh_programID, glm::mat4 MatriuVista) {
    if (!model) return;

    float offsetX = 0.0f, offsetY = 0.0f, offsetZ = 0.0f;
    float sepX = 0.0f, sepY = 0.0f, sepZ = 0.0f;
    float escala = 0.0f, y = 0.0f, rad = 90.0f, z = 0.0f, x = 1.0f;

    glm::mat4 ModelMatrix = glm::mat4(1.0f);
    glm::mat4 BaseCarMatrix = ModelMatrix;
    glm::mat4 CarOrientationMatrix = ModelMatrix;

    float scaleFactor = 10.5f;
    offsetX = 55.0f; //rojo
    offsetY = -5.0f; //verde
    offsetZ = -43.0f; //azul
    escala = 0.8f;

    //  TRANSFORMACIONES DEL COCHE 
    float inclinacion = glm::radians(-8.0f);  
    glm::mat4 InclinationMatrix = glm::rotate(glm::mat4(1.0f), inclinacion, glm::vec3(1.0f, 0.0f, 0.0f));
     
    glm::mat4 TransMatrix = glm::translate(ModelMatrix, glm::vec3(offsetX, offsetY, offsetZ));
    glm::mat4 RotMatrix = glm::rotate(TransMatrix, glm::radians(rad), glm::vec3(x, y, z));
     
    CarOrientationMatrix = InclinationMatrix * RotMatrix;
     
    glm::mat4 ModelMatrixCar = glm::scale(CarOrientationMatrix, glm::vec3(escala));
    glm::mat4 NormalMatrixCar = glm::transpose(glm::inverse(MatriuVista * ModelMatrixCar));

    //  DIBUJAR COCHE 
    glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrixCar[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrixCar[0][0]);
    model->draw_TriVAO_OBJ(sh_programID);

    //  DIBUJAR RUEDAS QUE SIGUEN AL COCHE 
    if (model_rueda) {
        glm::vec3 pos_rueda_DI = glm::vec3(-0.76f, -0.4f, -0.15f);
        glm::vec3 pos_rueda_DD = glm::vec3(0.25f, -0.4f, -0.10f);
        glm::vec3 pos_rueda_TI = glm::vec3(-0.85f, -0.4f, 1.80f);
        glm::vec3 pos_rueda_TD = glm::vec3(0.14f, -0.4f, 1.85f); 
        escala = 1.0;
        glm::mat4 ModelMatrixCar = glm::scale(CarOrientationMatrix, glm::vec3(escala));
         
        ModelMatrixCar = glm::rotate(ModelMatrixCar, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 NormalMatrixCar = glm::transpose(glm::inverse(MatriuVista * ModelMatrixCar));
         
        //  Dibujar ruedas 
        auto dibujarRueda = [&](glm::vec3 pos, glm::vec3 offsetLocal) {
            glm::mat4 ModelMatrixRueda = ModelMatrixCar;   // Hereda todas las transformaciones del coche
            ModelMatrixRueda = glm::translate(ModelMatrixRueda, pos + offsetLocal);  // Posición + desplazamiento local

            glm::mat4 NormalMatrixRueda = glm::transpose(glm::inverse(MatriuVista * ModelMatrixRueda));
            glUniformMatrix4fv(glGetUniformLocation(sh_programID, "modelMatrix"), 1, GL_FALSE, &ModelMatrixRueda[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(sh_programID, "normalMatrix"), 1, GL_FALSE, &NormalMatrixRueda[0][0]);
            model_rueda->draw_TriVAO_OBJ(sh_programID);
            };

        float movAtras = -9.0f;   
        float movDer = 2.9f;    
        //(verde,azul,rojo)
        dibujarRueda(pos_rueda_DI, glm::vec3(0.0f, -1.0f, 0.6f));
        dibujarRueda(pos_rueda_DD, glm::vec3(movDer, -1.0f, 0.7f));
        dibujarRueda(pos_rueda_TI, glm::vec3(0.35f, -1.2f, movAtras-0.05));
        dibujarRueda(pos_rueda_TD, glm::vec3(movDer+0.45, -1.2f, movAtras+0.1));

    }

}
