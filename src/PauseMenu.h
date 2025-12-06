#pragma once
#include "IMenuState.h"

class PauseMenuState : public IMenuState {
public:
    void Render(MenuController& controller);
};