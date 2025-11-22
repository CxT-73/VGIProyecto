#include "zones.h"   

Zones::Zones(const std::string& nombreObjeto) {

    std::vector<glm::vec3> coords = {
        { -400.0f, 1000.0f, 180.0f }, // zona 0
        { 500.0f, 630.0f, 150.0f }, //zona 1
        { 600.0f, 100.0f, 100.0f }, //zona 2
        { -35.0f, 150.0f, 40.0f }, // zona 3
        { 1000.0f, 50.0f, 140.0f }, // zona 4
        { 550.0f, -350.0f, 60.0f }, // zona 5
        { -350.0f, 600.0f, 100.0f }, // zona 6
        { -650.0f, 600.0f, 100.0f },// zona 7
        { -650.0f, 950.0f, 180.0f } // zona 8
    };

    for (const glm::vec3& pos : coords) {
        OBJ* nuevo = new OBJ(nombreObjeto);
        puntos.push_back(nuevo);
        posiciones.push_back(pos);
    }
}

void Zones::renderTodos(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, CColor col_object, bool sw_mat[5]){

    for (size_t i = 0; i < posiciones.size(); ++i) {
        glm::vec3 escala = glm::vec3(1.0f, 1.0f, 1.0f);  

        glm::mat4 MatriuTG_local =
            glm::translate(MatriuTG, posiciones[i]) *
            glm::scale(glm::mat4(1.0f), escala);

        puntos[i]->render(sh_programID, MatriuVista, MatriuTG_local, col_object, sw_mat);
    }
}

Zones::~Zones() {
    for (OBJ* p : puntos) delete p;
}

 