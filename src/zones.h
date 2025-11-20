#pragma once

#include "stdafx.h" 
#include "objLoader.h"

class COBJModel;
 

class Zones {   
public:
    std::vector<OBJ*> puntos;
    std::vector<glm::vec3> posiciones;
    std::vector<glm::vec3> coords;
    COBJModel* zona;
    std::string nom;
    OBJ* modelo;
    Zones(const std::string& nombreObjeto);
    glm::vec3 getPosicion(int index) const {
        if (index >= 0 && index < posiciones.size())
            return posiciones[index];
        return glm::vec3(0.0f); 
    }

    ~Zones();                        
    void renderTodos(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, CColor col_object, bool sw_mat[5]);
 
};
  
