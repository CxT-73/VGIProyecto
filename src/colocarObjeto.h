#pragma once

#include "stdafx.h" 
#include "objLoader.h"
#include "zones.h"
#include "coche.h"

class COBJModel;

class ObjetoSeguidor { 
public:
    OBJ* objeto;
    Zones* zonas;
    int zonaIndex; 
    bool invisible = false;

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

    void render(GLuint sh_programID, glm::mat4 MatriuVista, glm::mat4 MatriuTG, CColor col_object, bool sw_mat[5]){
        if (invisible) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            CColor invisibleColor = { 1.0f, 1.0f, 1.0f, 0.0f };
            objeto->render(sh_programID, MatriuVista, MatriuTG, invisibleColor, sw_mat);
        }
        else {
            objeto->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);
        }
    }

};
