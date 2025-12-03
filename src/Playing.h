#pragma once
// Playing.h
#pragma once

#include "MenuController.h" // Necesario para IMenuState y MenuController

class PlayingState : public IMenuState {
public:
    // El estado de juego no suele renderizar botones, solo el HUD.
    void Render(MenuController& controller) override;
};