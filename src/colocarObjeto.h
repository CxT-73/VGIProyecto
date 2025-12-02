#pragma once

#include "stdafx.h" 
#include "objLoader.h"
#include "zones.h"
#include "coche.h"

class COBJModel;

class ObjetoSeguidor { 
public:
    OBJ* objeto;
    ObjetoSeguidor* objeto_seguidor;
    Zones* zonas;
    int zonaIndex;  

    ObjetoSeguidor(OBJ* obj, Zones* zs, int idx)
        : objeto(obj), zonas(zs), zonaIndex(idx) {
    }
     
    void colocarEnZona(glm::vec3 offset, glm::vec3 rot, glm::vec3 scl) {
        glm::vec3 origen = zonas->getPosicion(zonaIndex);
        glm::vec3 posicionFinal = origen + offset; 
        objeto->setTransform(posicionFinal, rot, scl);
    }
     
    void colocarBajoCoche(Coche* coche, glm::vec3 offset, glm::vec3 rot, glm::vec3 scl) {
        glm::vec3 bajoCoche = glm::vec3(coche->x, coche->y, coche->z) + offset;
        objeto->setTransform(bajoCoche, rot, scl);
    }
     
    void colocarDuplicadosEnZonas(const std::map<int, std::pair<int, std::vector<glm::vec3>>>& zonasConfig, const std::map<int, std::vector<int>>& invisiblesPorZona, glm::vec3 rot, glm::vec3 scl, GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, CColor col_object, bool sw_mat[5]);
};
