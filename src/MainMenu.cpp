#include "MainMenu.h"
#include "NewGame.h"
#include "Controls.h"
#include "Credits.h"

void MainMenuState::Render(MenuController& controller) {
    controller.n = 1;
    char* cameraPtr = controller.GetCameraPtr();
    if (cameraPtr) {
        *cameraPtr = 'I';
    }

    controller.setState("Main");

    controller.DrawBackgroundOverlay();

    // 3 botones + 2 espaciados (SPACING) + 1 espaciado (Título) + Título (aprox 2x BUTTON_HEIGHT)
    float total_content_height = 3.0f * BUTTON_HEIGHT + 2.0f * SPACING + 2 * SPACING + 2.0f * BUTTON_HEIGHT;

    // Iniciar ventana, centrarla y obtener la posición X
    float centered_x_pos = controller.BeginButtonWindow("MainMenuButtons", total_content_height);

    // --- TÍTULO (Escala 2x) ---
    ImGui::SetWindowFontScale(2.0f);
    const char* title = u8"--- Menú principal ---";
    float text_width = ImGui::CalcTextSize(title).x;

    // Centrar el título dentro del área de la ventana
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) * 0.5f);
    ImGui::TextColored(NEON_CYAN_TITLE, title);

    ImGui::Spacing(); ImGui::Spacing(); // Espacio después del título
    ImGui::SetWindowFontScale(1.0f); // Volver a escala 1x para el resto

    // Posicionar el cursor X para el contenido (botones)
    ImGui::SetCursorPosX(centered_x_pos);

    controller.PushUserNeonStyle();
    ImVec2 button_size(BUTTON_WIDTH, BUTTON_HEIGHT);

    if (ImGui::Button("Jugar", button_size)) {
        controller.SwitchState(new NewGameState());
    }
    ImGui::Spacing(); ImGui::Spacing(); ImGui::SetCursorPosX(centered_x_pos);

    if (ImGui::Button("Opcions", button_size)) {
        controller.SwitchState(new NewGameState());
    }

    ImGui::Spacing(); ImGui::Spacing(); ImGui::SetCursorPosX(centered_x_pos);

    if (ImGui::Button("Controls", button_size)) { 
        controller.SwitchState(new ControlsState());
    }

    ImGui::Spacing(); ImGui::Spacing(); ImGui::SetCursorPosX(centered_x_pos);

    if (ImGui::Button(u8"Crèdits", button_size)) { 
        controller.SwitchState(new CreditsState());
    }
    ImGui::Spacing(); ImGui::Spacing(); ImGui::SetCursorPosX(centered_x_pos);

    if (ImGui::Button("Sortir", button_size)) {
        controller.GetContext()->shouldCloseApp = true;
    }

    controller.PopUserNeonStyle();
    ImGui::End();
}