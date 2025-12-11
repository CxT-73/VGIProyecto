#pragma once
#include "ImGui\imgui.h"
#include <vector>
#include <string>
#include "LinearMath/btVector3.h"

// Estructura para compartir datos entre el juego y los menús
struct GameContext {
    int selectedCar = 0;
    int selectedCircuit = 0;

    // Estadísticas para el End Menu
    float finalTime = 0.0f;
    int collisionCount = 0;
    int score = 0;
    float gameTime = 0.0f;        // Tiempo transcurrido en segundos
    int carHealth = 10;

    // Control de flujo
    bool isGameRunning = false; // Para saber si "Resume" tiene sentido
    bool shouldCloseApp = false;
    bool resetGame = false; // Flag para reiniciar variables
    std::string state = "Main";

};