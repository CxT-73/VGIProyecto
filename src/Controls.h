#pragma once
#include "GameContext.h" // Asumiendo que guardaste el struct anterior aquí
#include "IMenuState.h"
#include <iostream> 

class ControlsState : public IMenuState {
public:
    void Render(MenuController& controller) override;
};
