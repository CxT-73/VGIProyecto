#include "PauseMenu.h"
#include "MainMenu.h"
#include "Playing.h"

void PauseMenuState::Render(MenuController& controller) {

    char* cameraPtr = controller.GetCameraPtr();
    if (cameraPtr) {
        *cameraPtr = 'A';
    }

    controller.setState("Paused");

    controller.DrawBackgroundOverlay();
    controller.GetContext()->isGameRunning = false;

    // 3 Botones + 2 espaciados (SPACING) + 1 espaciado (Título) + Título (aprox 1x BUTTON_HEIGHT)
    float title_height = ImGui::GetTextLineHeight() * 1.5f;
    float total_content_height = 3.0f * BUTTON_HEIGHT + 2.0f * SPACING + SPACING + title_height;

    float centered_x_pos = controller.BeginButtonWindow("PauseMenuButtons", total_content_height);

    // Título
    float text_width = ImGui::CalcTextSize("Paused Practice").x;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) * 0.5f);
    ImGui::TextColored(NEON_CYAN_TITLE, "Paused Practice");
    ImGui::Spacing(); ImGui::Spacing();
    ImGui::SetCursorPosX(centered_x_pos);

    controller.PushUserNeonStyle();
    ImVec2 button_size(BUTTON_WIDTH, BUTTON_HEIGHT);

    if (ImGui::Button("Resume practice", button_size)) {
        char* cameraPtr = controller.GetCameraPtr();
        if (cameraPtr) {
            *cameraPtr = 'F'; 
        }
        controller.GetContext()->isGameRunning = true;
        controller.SwitchState(new PlayingState());
    }
    ImGui::Spacing(); ImGui::Spacing(); ImGui::SetCursorPosX(centered_x_pos);

    if (ImGui::Button("Options", button_size)) {}
    ImGui::Spacing(); ImGui::Spacing(); ImGui::SetCursorPosX(centered_x_pos);

    if (ImGui::Button("Exit to menu", button_size)) {
        controller.GetContext()->isGameRunning = false;
        controller.SwitchState(new MainMenuState());
    }

    controller.PopUserNeonStyle();
    ImGui::End();
}