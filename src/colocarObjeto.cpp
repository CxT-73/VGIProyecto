#include "colocarObjeto.h"   
#include "zones.h"


void ObjetoSeguidor::colocarDuplicadosEnZonas(
    const std::map<int, std::pair<int, std::vector<glm::vec3>>>& zonasConfig,
    const std::map<int, std::vector<int>>& invisiblesPorZona,  
    glm::vec3 rot,
    glm::vec3 scl,
    GLuint sh_programID,
    glm::mat4 MatriuVista,
    glm::mat4 MatriuTG,
    CColor col_object,
    bool sw_mat[5])
{
    for (const auto& par : zonasConfig) {
        int idx = par.first;
        int cantidad = par.second.first;
        const auto& offsets = par.second.second;

        glm::vec3 origen = zonas->getPosicion(idx);

        for (int i = 0; i < cantidad; ++i) {
            glm::vec3 offset = (i < offsets.size()) ? offsets[i] : glm::vec3(0.0f);
            glm::vec3 posicionFinal = origen + offset;

            objeto->setTransform(posicionFinal, rot, scl);
             
            bool esInvisible = false;
            auto it = invisiblesPorZona.find(idx);
            if (it != invisiblesPorZona.end()) {
                const auto& invisibles = it->second;
                esInvisible = std::find(invisibles.begin(), invisibles.end(), i) != invisibles.end();
            }

            if (esInvisible) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                CColor invisibleColor = { 1.0f, 1.0f, 1.0f, 0.0f };
                objeto->render(sh_programID, MatriuVista, MatriuTG, invisibleColor, sw_mat);
            }
            else {
                objeto->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);
            }
        }
    }
}
