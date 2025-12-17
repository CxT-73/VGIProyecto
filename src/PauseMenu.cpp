#include "PauseMenu.h"
#include "MainMenu.h"
#include "Playing.h" 
#include "Controls.h"

void PauseMenuState::Render(MenuController& controller) {
    PlaySound(NULL, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    controller.n = 2;
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
    const char* title = u8"-- Partida en pausa --";
    float text_width = ImGui::CalcTextSize(title).x;

    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) * 0.5f);
    ImGui::TextColored(NEON_CYAN_TITLE, title);

    ImGui::Spacing(); ImGui::Spacing();
    ImGui::SetCursorPosX(centered_x_pos);

    controller.PushUserNeonStyle();
    ImVec2 button_size(BUTTON_WIDTH, BUTTON_HEIGHT);

    if (ImGui::Button("Reprendre la partida", button_size)) {
        char* cameraPtr = controller.GetCameraPtr();
        if (cameraPtr) {
            *cameraPtr = 'F'; 
        }
        controller.GetContext()->isGameRunning = true;
        controller.SwitchState(new PlayingState());
        PlaySound(TEXT("motor.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    }
    ImGui::Spacing(); ImGui::Spacing(); ImGui::SetCursorPosX(centered_x_pos); 

    if (ImGui::Button("Controls", button_size)) {
        controller.SwitchState(new ControlsState());
    }
    ImGui::Spacing(); ImGui::Spacing(); ImGui::SetCursorPosX(centered_x_pos);

    if (ImGui::Button(u8"Sortir al Menú principal", button_size)) {
        controller.GetContext()->isGameRunning = false;
		controller.setRestart();
        controller.SwitchState(new MainMenuState());
    }

    controller.PopUserNeonStyle();
    ImGui::End();
}