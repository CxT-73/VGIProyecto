#pragma once

#include "zones.h"
#include "objLoader.h"
#include <map>

#include <vector>
class ObjetoSeguidor {
public:
    OBJ* objeto;
    Zones* zonas;
    int zonaIndex;
    btDiscreteDynamicsWorld* mundo;
    std::vector<glm::vec3> posicionesDuplicados;

    std::vector<OBJ*> duplicados;
    std::vector<int> invisibleFlags;

    ObjetoSeguidor(OBJ* base, Zones* zs, int idx, btDiscreteDynamicsWorld* m);

    void crearDuplicados(
        const std::map<int, std::pair<int, std::vector<glm::vec3>>> &zonasConfig,
        const std::map<int, std::vector<int>>& invisiblesPorZona,
        glm::vec3 rot,
        glm::vec3 escala);

    void renderDuplicados(GLuint sh_programID,
        glm::mat4 MatriuVista,
        glm::mat4 MatriuTG,
        CColor col_object,
        bool sw_mat[5]);

    void cleanup();
};