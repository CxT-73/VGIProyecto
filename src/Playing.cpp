// Playing.cpp

#include "Playing.h"
#include <cstdio>
// ¡CLAVE! Necesario para el centrado manual del texto en la barra de vida
#include "ImGui/imgui_internal.h" 

// Definiciones de Color
#define HUD_COLOR_TIME ImVec4(0.0f, 0.0f, 0.0f, 1.0f)     // Negro
#define HEALTH_COLOR_OK ImVec4(0.0f, 1.0f, 0.0f, 1.0f)  // Verde
#define HEALTH_COLOR_WARN ImVec4(1.0f, 1.0f, 0.0f, 1.0f) // Amarillo
#define HEALTH_COLOR_LOW ImVec4(1.0f, 0.0f, 0.0f, 1.0f) // Rojo
#define TEXT_COLOR_INSIDE_BAR ImVec4(0.0f, 0.0f, 0.0f, 1.0f) // Negro, para el porcentaje

void PlayingState::Render(MenuController& controller) {

    // Asignar cámara de juego
    char* cameraPtr = controller.GetCameraPtr();
    if (cameraPtr == nullptr) {
        // Asumiendo que CAM_FOLLOW es la cámara de carrera
        *cameraPtr = 'F';
    }

    controller.setState("Playing");


    ImGuiIO& io = ImGui::GetIO();
    float display_w = io.DisplaySize.x;
    float display_h = io.DisplaySize.y;
    GameContext* context = controller.GetContext();

    // -----------------------------------------------------
    // Banderas Comunes: Limpio, No-interactivo, Sin Fondo
    // -----------------------------------------------------
    ImGuiWindowFlags common_flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoBackground;

    // Quitar borde y padding para ventanas del HUD
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    // =====================================================
    // 1. TEMPORIZADOR (Centro Arriba)
    // =====================================================

    // Dimensiones de la ventana contenedora
    const float TIMER_WINDOW_WIDTH = 400.0f;
    const float TIMER_WINDOW_HEIGHT = 100.0f;
    const float TIME_SCALE = 3.5f;

    // 1. Formatear el tiempo (MM:SS.ms)
    int minutes = (int)context->gameTime / 60;
    float seconds = context->gameTime - (minutes * 60);
    char timeBuffer[32];
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%05.2f", minutes, seconds);

    // 2. Calcular posición
    ImVec2 window_pos(
        (display_w / 2.0f) - (TIMER_WINDOW_WIDTH / 2.0f),
        10.0f
    );

    ImGui::SetNextWindowPos(window_pos);
    ImGui::SetNextWindowSize(ImVec2(TIMER_WINDOW_WIDTH, TIMER_WINDOW_HEIGHT));

    // 3. Dibujar
    if (ImGui::Begin("##TimerHUD", nullptr, common_flags)) {

        // Centrado de texto dentro de la ventana de tamaño fijo
        ImGui::SetWindowFontScale(TIME_SCALE);
        float text_width = ImGui::CalcTextSize(timeBuffer).x;

        ImGui::SetCursorPosX((TIMER_WINDOW_WIDTH - text_width) * 0.5f);
        ImGui::SetCursorPosY(TIMER_WINDOW_HEIGHT * 0.5f - (ImGui::GetTextLineHeight() * 0.5f));

        ImGui::PushStyleColor(ImGuiCol_Text, HUD_COLOR_TIME); // Color Negro
        ImGui::Text("%s", timeBuffer);
        ImGui::PopStyleColor();

        ImGui::SetWindowFontScale(1.0f);
        ImGui::End();
    }

    // =====================================================
    // 2. VIDA DEL COCHE (Centro Abajo) - Porcentaje Centrado Manual
    // =====================================================

    float healthRatio = (float)context->carHealth / 100.0f;
    float bar_width = 400.0f;
    float bar_height = 30.0f;

    // Calcular color
    ImVec4 healthColor = HEALTH_COLOR_OK;
    if (context->carHealth <= 30) {
        healthColor = HEALTH_COLOR_LOW;
    }
    else if (context->carHealth <= 60) {
        healthColor = HEALTH_COLOR_WARN;
    }

    // Posición: Centro X, Margen inferior Y
    ImVec2 health_pos(
        (display_w / 2.0f) - (bar_width / 2.0f),
        display_h - bar_height - 30.0f
    );

    ImGui::SetNextWindowPos(health_pos);
    ImGui::SetNextWindowSize(ImVec2(bar_width, bar_height));

    if (ImGui::Begin("##HealthHUD", nullptr, common_flags)) {

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, healthColor);

        // Formatear el texto: Solo porcentaje
        char healthText[32];
        snprintf(healthText, sizeof(healthText), "%d%%", context->carHealth);

        // 1. Dibujar la barra de progreso SIN etiqueta (label = "")
        // La barra ocupará todo el área de la ventana
        ImGui::ProgressBar(healthRatio, ImVec2(bar_width, bar_height), "");

        // 2. Obtener la posición de la barra
        ImVec2 bar_min = ImGui::GetItemRectMin();

        // 3. Calcular la posición central del texto
        ImVec2 text_size = ImGui::CalcTextSize(healthText);

        ImVec2 text_pos(
            bar_min.x + (bar_width - text_size.x) * 0.5f,
            bar_min.y + (bar_height - text_size.y) * 0.5f
        );

        // 4. Dibujar el texto directamente sobre la barra (ImGui_internal.h)
        ImGui::GetWindowDrawList()->AddText(
            text_pos,
            ImGui::GetColorU32(TEXT_COLOR_INSIDE_BAR), // Texto Negro
            healthText
        );

        ImGui::PopStyleColor();
        ImGui::End();
    }

    // Quitar estilos Pushados
    ImGui::PopStyleVar(2);
    // =====================================================
    // 3. VELOCIDAD DEL COCHE
    // =====================================================
    const float VELOCITY_WINDOW_WIDTH = 400.0f;
    const float VELOCITY_WINDOW_HEIGHT = 100.0f;
    const float VEL_SCALE = 3.5f;

    // 1. Obtener datos
    float velocitat = controller.GetContext()->velocity;
    char vBuffer[32];
    snprintf(vBuffer, sizeof(vBuffer), "%.0f MPH", velocitat);

    // 2. Calcular posición (BOTTOM-LEFT)
    float display_w_v = ImGui::GetIO().DisplaySize.x;
    float display_h_v = ImGui::GetIO().DisplaySize.y; // <--- Necesitamos la altura
    float padding = 20.0f; // Margen de separación del borde

    ImVec2 window_pos_v(
        padding,                                            // Izquierda (X)
        display_h_v - VELOCITY_WINDOW_HEIGHT - padding        // Abajo (Y)
    );

    ImGui::SetNextWindowPos(window_pos_v);
    ImGui::SetNextWindowSize(ImVec2(VELOCITY_WINDOW_WIDTH, VELOCITY_WINDOW_HEIGHT));

    // Flags transparentes
    ImGuiWindowFlags hud_flags = ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoBackground;

    // 3. Dibujar
    if (ImGui::Begin("##VelocityHUD", nullptr, hud_flags)) {

        ImGui::SetWindowFontScale(VEL_SCALE);

        // Centrar texto en la ventana
        float text_width = ImGui::CalcTextSize(vBuffer).x;
        ImGui::SetCursorPosX((VELOCITY_WINDOW_WIDTH - text_width) * 0.5f);
        ImGui::SetCursorPosY((VELOCITY_WINDOW_HEIGHT - ImGui::GetTextLineHeight()) * 0.5f);

        // Color Cyan Neón
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Text("%s", vBuffer);
        ImGui::PopStyleColor();

        ImGui::SetWindowFontScale(1.0f);
    }
    ImGui::End();
}

