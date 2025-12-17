#pragma once
#include "IMenuState.h"

class NewGameState : public IMenuState {
    const char* carOptions[3] = { "Deportivo Rojo", "Camioneta Azul", "F1 Rapido" };
    const char* trackOptions[2] = { "Circuito Urbano", "Circuito Montaña" };
    const char* IluminacioOptions[5] = { "Amanecer", "Mediodia","Anochecer", "Atardecer-desierto", "Noche" }; 
public:
    void Render(MenuController& controller);
};