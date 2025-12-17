// EndGame.cpp

#include "EndGame.h"
#include "MainMenu.h"
#include <cstdio> // Para snprintf

// Define un color de acento adicional, por ejemplo, un verde neón
const ImVec4 NEON_GREEN_TITLE = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

void EndGameState::Render(MenuController& controller) {
    // 1. CAMBIO DE CÁMARA (Solo si es necesario)
    char* cameraPtr = controller.GetCameraPtr();
    if (cameraPtr && *cameraPtr != 'I') {
        *cameraPtr = 'I';
    }

    // 2. CÁLCULO DE PUNTUACIÓN
    // Calculamos solo si el score es 0 (para no recalcular lo mismo cada frame)
    if (controller.GetContext()->score == 0) {
        controller.calculateScore();
    }
    controller.DrawBackgroundOverlay();
    controller.setState("EndGame");

    // --- Lògica ImGui (alçada i finestra) ---
    float base_lh = ImGui::GetTextLineHeight();
    float total_content_height =
        (3.0f * base_lh) +
        (3.0f * SPACING) +
        BUTTON_HEIGHT +
        (3.0f * SPACING);

    float centered_x_pos =
        controller.BeginButtonWindow("EndGameButtons", total_content_height);
     
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 70.0f);

    controller.PushUserNeonStyle();

    // =====================================================
    // TÍTOL (centrat correctament)
    // =====================================================
    const char* title = u8"-- PARTIDA FINALITZADA --";
    float title_width = ImGui::CalcTextSize(title).x;

    // Centrat horitzontal
    ImGui::SetCursorPosX(
        (ImGui::GetWindowWidth() - title_width) * 0.5f
    ); 
    ImGui::TextColored(NEON_CYAN_TITLE, title);


    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();


    // Estadísticas (Mostramos los datos reales que vienen de escena.cpp)
    const float STATS_WIDTH = 350.0f;
    ImGui::SetCursorPosX(centered_x_pos + (BUTTON_WIDTH - STATS_WIDTH) * 0.5f);
    float timeTaken = controller.GetContext()->finalTime;

    ImGui::Text("TEMPS:");
    ImGui::SameLine(centered_x_pos + STATS_WIDTH * 0.5f);

    if (timeTaken >= 60.0f) {
        int minutes = static_cast<int>(timeTaken) / 60;
        int seconds = static_cast<int>(timeTaken) % 60;
        // Formato 00:00
        ImGui::TextColored(NEON_CYAN_TITLE, "%02d:%02d min", minutes, seconds);
    }
    else {
        ImGui::TextColored(NEON_CYAN_TITLE, "                   %.2f s", timeTaken);
    }

    ImGui::SetCursorPosX(centered_x_pos + (BUTTON_WIDTH - STATS_WIDTH) * 0.5f);
    ImGui::Text(u8"COL·LISIONS:"); ImGui::SameLine(centered_x_pos + STATS_WIDTH * 0.5f);
    ImGui::TextColored(NEON_CYAN_TITLE, "                           %d", controller.GetContext()->collisionCount);

    // Puntuación
    char score_buffer[64];
    snprintf(score_buffer, sizeof(score_buffer), "%d pts", controller.GetContext()->score);
    ImGui::SetCursorPosX(centered_x_pos + (BUTTON_WIDTH - STATS_WIDTH) * 0.5f);
    ImGui::Text(u8"PUNTUACIÓ FINAL:"); ImGui::SameLine();
    ImGui::TextColored(NEON_CYAN_TITLE, "     %s", score_buffer);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const char* resultText = nullptr;
    ImVec4 resultColor;

    if (controller.GetContext()->carHealth <= 0) {
        resultText = u8"VEHICLE DESTRUÏT";
        resultColor = ImVec4(1, 0, 0, 1);
    }
    else if (controller.GetContext()->score >= 5) {
        resultText = u8"PROVA SUPERADA - APTE";
        resultColor = ImVec4(0, 1, 0, 1);
    }
    else {
        resultText = u8"PUNTUACIÓ INSUFICIENT - NO APTE";
        resultColor = ImVec4(1, 0.5f, 0, 1);
    }

    float text_width = ImGui::CalcTextSize(resultText).x;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) * 0.5f);
    ImGui::TextColored(resultColor, "%s", resultText);

    // BOTÓN: Aquí es donde realmente queremos limpiar los datos al salir
    ImGui::SetCursorPosX(centered_x_pos);
    if (ImGui::Button(u8"Tornar al Menú Principal", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT))) {
        controller.setRestart(); // <--- LIMPIAMOS DATOS AQUÍ, AL SALIR
        controller.SwitchState(new MainMenuState());
    }

    controller.PopUserNeonStyle();
    ImGui::End();
}