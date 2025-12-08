#include "colocarObjeto.h" 
ObjetoSeguidor::ObjetoSeguidor(OBJ* base, Zones* zs, int idx, btDiscreteDynamicsWorld* m)
    : objeto(base), zonas(zs), zonaIndex(idx), mundo(m)
{
} 
void ObjetoSeguidor::crearDuplicados(
    const std::map<int, std::pair<int, std::vector<glm::vec3>>>& zonasConfig,
    const std::map<int, std::vector<int>>& invisiblesPorZona,
    glm::vec3 rot,
    glm::vec3 escala)
{
    COBJModel* sharedModel = objeto->objecteOBJ;

    for (auto& entry : zonasConfig)
    {
        int zonaID = entry.first;

        int cantidad = entry.second.first;
        auto offsets = entry.second.second;

        glm::vec3 zonaPos = zonas->getPosicion(zonaID); 

        for (int i = 0; i < cantidad; i++)
        {
            glm::vec3 offset = (i < offsets.size()) ? offsets[i] : glm::vec3(0);
            glm::vec3 posFinal = zonaPos + offset; 

            OBJ* copia = new OBJ(objeto->nom, posFinal, sharedModel);
            copia->setTransform(posFinal, rot, escala);
            copia->initFisicas(mundo, offset);

            duplicados.push_back(copia);
            posicionesDuplicados.push_back(posFinal);
            bool invisible = false;
            auto it = invisiblesPorZona.find(zonaID);
            if (it != invisiblesPorZona.end())
                invisible = (std::find(it->second.begin(), it->second.end(), i) != it->second.end());

            invisibleFlags.push_back(invisible ? 1 : 0);
             
        }
    }
}

void ObjetoSeguidor::renderDuplicados(
    GLuint sh_programID,
    glm::mat4 MatriuVista,
    glm::mat4 MatriuTG,
    CColor col_object,
    bool sw_mat[5])
{
    for (int i = 0; i < duplicados.size(); i++)
    {
        OBJ* obj = duplicados[i];
        glm::vec3 pos = posicionesDuplicados[i]; 

        if (invisibleFlags[i] == 1)
        {
            CColor invisibleColor = { col_object.r, col_object.g, col_object.b, 0.0f };

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            obj->render(sh_programID, MatriuVista, MatriuTG, invisibleColor, sw_mat);

            glDisable(GL_BLEND);
        }
        else
        {
            obj->render(sh_programID, MatriuVista, MatriuTG, col_object, sw_mat);
        }
    }
}



void ObjetoSeguidor::cleanup()
{
    for (auto obj : duplicados)
    {
        obj->destroyFisicas(mundo);
        delete obj;
    }
    duplicados.clear();
    invisibleFlags.clear();
}
