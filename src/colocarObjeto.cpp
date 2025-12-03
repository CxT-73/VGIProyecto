#include "colocarObjeto.h"
#include <iostream>
ObjetoSeguidor::ObjetoSeguidor(OBJ* base, Zones* zs, int idx, btDiscreteDynamicsWorld* m)
    : objeto(base), zonas(zs), zonaIndex(idx), mundo(m)
{
}
void ObjetoSeguidor::crearDuplicados(
    const std::map<int, std::pair<int, std::vector<glm::vec3>>> &zonasConfig,
    const std::map<int, std::vector<int>>& invisiblesPorZona,
    glm::vec3 rot,
    glm::vec3 escala)
{
    std::cout << "[DEBUG] Entrando en crearDuplicados()" << std::endl;

    COBJModel* sharedModel = objeto->objecteOBJ;

    for (auto& entry : zonasConfig)
    {
        int zonaID = entry.first;
        std::cout << "[DEBUG] Zona encontrada: " << zonaID << std::endl;

        int cantidad = entry.second.first;
        auto offsets = entry.second.second;

        glm::vec3 zonaPos = zonas->getPosicion(zonaID);

        for (int i = 0; i < cantidad; i++)
        {

            glm::vec3 offset = (i < offsets.size()) ? offsets[i] : glm::vec3(0);
            std::cout << "[DEBUG]   zonaPos = "
                << zonaPos.x << ", "
                << zonaPos.y << ", "
                << zonaPos.z << std::endl;

            std::cout << "[DEBUG]   offset = "
                << offset.x << ", "
                << offset.y << ", "
                << offset.z << std::endl;

            glm::vec3 posFinal =  offset;
            objeto->posicion = offset;
            std::cout << "[DEBUG]   Creando duplicado " << i
                << " en zona " << zonaID << std::endl;

            std::cout << "          Pos final = "
                << posFinal.x << ", "
                << posFinal.y << ", "
                << posFinal.z << std::endl;
            std::cout << "[INIT] Duplicado creado en: "
                << posFinal.x << ", "
                << posFinal.y << ", "
                << posFinal.z << std::endl;

            OBJ* copia = new OBJ(objeto->nom, posFinal, sharedModel);
            copia->setTransform(posFinal, rot, escala);

            std::cout << "          Llamando a initFisicas()" << std::endl;
            copia->initFisicas(mundo, offset);

            duplicados.push_back(copia);

            bool invisible = false;
            auto it = invisiblesPorZona.find(zonaID);
            if (it != invisiblesPorZona.end())
                invisible = (std::find(it->second.begin(), it->second.end(), i) != it->second.end());

            invisibleFlags.push_back(invisible ? 1 : 0);
        }
    }
}

void ObjetoSeguidor::renderDuplicados(GLuint sh_programID,
    glm::mat4 MatriuVista,
    glm::mat4 MatriuTG,
    CColor col_object,
    bool sw_mat[5])
{

    for (size_t i = 0; i < duplicados.size(); i++)
    {
        OBJ* obj = duplicados[i];
        std::cout << "[RENDER] Dibujando duplicado en "
            << obj->posicion.x << ", "
            << obj->posicion.y << ", "
            << obj->posicion.z << "\n";

        // Sincronizar física ? render
        if (obj->m_rigidBody)
        {
            btTransform t;
            obj->m_rigidBody->getMotionState()->getWorldTransform(t);
            btVector3 p = t.getOrigin();
            obj->posicion = glm::vec3(p.x(), p.y(), p.z());
        }

        bool invisible = (invisibleFlags[i] == 1);

        if (invisible)
        {
            CColor invisibleColor = { 1,1,1,0 };
            obj->render(sh_programID, MatriuVista, MatriuTG, invisibleColor, sw_mat);
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
