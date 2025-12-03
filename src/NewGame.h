#pragma once
#include "IMenuState.h"

class NewGameState : public IMenuState {
    const char* carOptions[3] = { "Deportivo Rojo", "Camioneta Azul", "F1 Rapido" };
    const char* trackOptions[2] = { "Circuito Urbano", "Circuito Montaña" };
public:
    void Render(MenuController& controller);
};