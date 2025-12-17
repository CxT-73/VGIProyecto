#include "NewGame.h"
#include "MainMenu.h"
#include "Playing.h" 

void NewGameState::Render(MenuController& controller) {

    char* cameraPtr = controller.GetCameraPtr();
    if (cameraPtr) {
        *cameraPtr = 'F';
    }

    controller.setState("NewGame");

    controller.DrawBackgroundOverlay();

    // Cálculo de altura
    float total_content_height = 2.0f * ImGui::GetTextLineHeight() + 2.0f * BUTTON_HEIGHT + 2.0f * BUTTON_HEIGHT + 5.0f * SPACING;

    float centered_x_pos = controller.BeginButtonWindow("NewGameButtons", total_content_height);

    controller.PushUserNeonStyle();

    // --- Título ---
    const char* title = u8"-- CONFIGURACIÓ --";
    float title_width = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - title_width) * 0.5f);
    ImGui::TextColored(NEON_CYAN_TITLE, title);

    ImGui::Spacing();

    // --- SOLUCIÓN: Centrado del Contenido y Altura ---

    // 1. Altura: Padding vertical a 17.0f (para altura ~50px)
    // 2. Centrado: Padding horizontal a 40.0f (CLAVE: Empuja el texto desde la izquierda)
    ImVec2 original_padding = ImGui::GetStyle().FramePadding;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(40.0f, 17.0f));

    // VEHÍCULO
    ImGui::Text("Vehicle:");
    ImGui::SetCursorPosX(centered_x_pos);

    ImGui::PushItemWidth(BUTTON_WIDTH);

    // El texto ahora será blanco (Neon) sobre el fondo oscuro del selector Y el fondo oscuro del desplegable.
    ImGui::Combo("##Coche", &controller.GetContext()->selectedCar, carOptions, IM_ARRAYSIZE(carOptions));

    ImGui::PopItemWidth();
    ImGui::Spacing(); ImGui::SetCursorPosX(centered_x_pos);

    // CIRCUITO
    ImGui::Text("Circuit:");
    ImGui::SetCursorPosX(centered_x_pos);

    ImGui::PushItemWidth(BUTTON_WIDTH);
    ImGui::Combo("##Circuito", &controller.GetContext()->selectedCircuit, trackOptions, IM_ARRAYSIZE(trackOptions));
    ImGui::PopItemWidth();

    // --- FIN DEL AJUSTE DE ESTILO LOCAL ---
    ImGui::PopStyleVar(); // Quita FramePadding (Centrado y Altura)

    // --- Botones ---
    ImGui::Spacing(); ImGui::Spacing(); ImGui::SetCursorPosX(centered_x_pos);

    ImVec2 button_size(BUTTON_WIDTH, BUTTON_HEIGHT);
    if (ImGui::Button(u8"Començar partida", button_size)) {
   
        controller.GetContext()->isGameRunning = true;
        controller.GetContext()->resetGame = true;
        controller.GetContext()->gameTime = 0.0f;    // Reiniciar temporizador
        controller.GetContext()->carHealth = 100;    // Vida inicial

        // Cambiar al nuevo estado
        controller.SwitchState(new PlayingState());
    }
    ImGui::Spacing(); ImGui::SetCursorPosX(centered_x_pos);
    if (ImGui::Button("Enrere", button_size)) {
        controller.SwitchState(new MainMenuState());
    }

    controller.PopUserNeonStyle();
    ImGui::End();
}