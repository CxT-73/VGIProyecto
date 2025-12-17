#pragma once
#include "GameContext.h" // Asumiendo que guardaste el struct anterior aquí
#include "IMenuState.h"
#include <iostream>

const float BUTTON_WIDTH = 380.0f;
const float BUTTON_HEIGHT = 75.0f;
const float SPACING = 50.0f;
const float MARGIN = 50.0f;
const ImVec4 NEON_CYAN_TITLE = ImVec4(0.0f, 0.85f, 1.0f, 1.0f); // Tono cian brillante
const ImVec4 HUD_COLOR_TIME = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Negro

class MenuController {
private:

    IMenuState* currentState = nullptr;
    GameContext* contextData;
    char* externalCameraPtr = nullptr;
public:

    int n = 0; // 1: menu principal, 2: menu pausa
    MenuController(GameContext* context, char* cameraPtr);

    ~MenuController();

    char* GetCameraPtr() { return externalCameraPtr; }

    void SwitchState(IMenuState* newState);

    void Render();

    GameContext* GetContext();

    void DrawBackgroundOverlay();

    float BeginButtonWindow(const char* name, float required_content_height, float width = BUTTON_WIDTH + MARGIN);

    void PushUserNeonStyle();

    void PopUserNeonStyle();

    // Add this method to allow access to the current state
    IMenuState* GetCurrentState() const { return currentState; }

    int getCarHealth() const {
        return contextData->carHealth;
	}

    bool getRestart() const {
        return contextData->resetGame;
    }

    void calculateScore();
    void setRestart() {
		contextData->isGameRunning = false;
		contextData->carHealth = 10;
		contextData->gameTime = 0.0f;
		contextData->collisionCount = 0;
		contextData->finalTime = 0.0f;
		contextData->score = 0;
    }

    std::string getState() const {
        return contextData->state;
	}

    void setState(std::string state) {
        contextData->state = state;
    }
    void loseHP(int amount) {
        contextData->carHealth -= amount;
        if (contextData->carHealth < 0) {
            contextData->carHealth = 0;
        }
	}
};