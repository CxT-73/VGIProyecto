#include "zones.h"   

Zones::Zones(const std::string& nombreObjeto) {

    std::vector<glm::vec3> coords = {
        { 500.0f, 660.0f, 0.0f },
        { 680.0f, 60.0f, 0.0f },
        { 70.0f, 100.0f, 0.0f },
        { 1320.0f, 80.0f, 30.0f },
        { 300.0f, -300.0f, 0.0f },
        { 0.0f, 0.0f, -10.0f },
        { 5.0f, 5.0f, 0.0f },
        { -5.0f, -5.0f, 0.0f },
        { -5.0f, -5.0f, 0.0f }
    };

    for (const glm::vec3& pos : coords) {
        OBJ* nuevo = new OBJ(nombreObjeto);
        puntos.push_back(nuevo);
        posiciones.push_back(pos);
    }
}

void Zones::renderTodos(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, CColor col_object, bool sw_mat[5]){

    for (size_t i = 0; i < posiciones.size(); ++i) {
        glm::mat4 MatriuTG_local = glm::translate(MatriuTG, posiciones[i]);
        puntos[i]->render(sh_programID, MatriuVista, MatriuTG_local, col_object, sw_mat);
    }
}

Zones::~Zones() {
    for (OBJ* p : puntos) delete p;
}

 