#pragma once
#include "IMenuState.h"

class EndGameState : public IMenuState {
public:
    void Render(MenuController& controller);
};