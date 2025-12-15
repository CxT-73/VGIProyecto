#pragma once
#include "IMenuState.h"

class EndGameState : public IMenuState {
public:
    void Render(MenuController& controller);
    void restartGame(MenuController& controller) { controller.setRestart(); };
};