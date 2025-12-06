#pragma once
#include "IMenuState.h"
#include "MenuController.h"

class MainMenuState : public IMenuState {
public:
    void Render(MenuController& controller);
};