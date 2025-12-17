#include "MenuController.h"
#include "Controls.h"
#include "MainMenu.h"
#include "PauseMenu.h"

inline void BulletLabel(const char* label, const char* text)
{
    ImGui::Bullet();
    ImGui::SameLine();
    ImGui::TextColored(NEON_CYAN_TITLE, "%s", label);
    ImGui::SameLine();
    ImGui::TextUnformatted(text);

    ImGui::Spacing();
}

inline void CenteredColumnTitle(const char* text)
{
    float column_width = ImGui::GetColumnWidth();
    float text_width = ImGui::CalcTextSize(text).x;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (column_width - text_width) * 0.5f);
    ImGui::TextColored(NEON_CYAN_TITLE, "%s", text);
}



void ControlsState::Render(MenuController& controller) {
    ImFont* boldFont;

    ImGuiIO& io = ImGui::GetIO();
    boldFont = io.Fonts->AddFontFromFileTTF(
        "fonts/Roboto-Bold.ttf",
        18.0f
    );

    controller.setState("Controls");
    controller.DrawBackgroundOverlay();
     
    float total_content_height =
        5.0f * BUTTON_HEIGHT + 4.0f * SPACING;

    float centered_x_pos =
        controller.BeginButtonWindow("ControlsWindow", 900, 1500);
     
    ImGui::SetWindowFontScale(2.0f);
    const char* title = u8"--- Controls ---"; 
    float text_width = ImGui::CalcTextSize(title).x; 
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_width) * 0.5f);
    ImGui::TextColored(NEON_CYAN_TITLE, title);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::SetWindowFontScale(1.0f);
     
    controller.PushUserNeonStyle();
    ImGui::BeginChild(
        "ControlsContent",
        ImVec2(0, -BUTTON_HEIGHT - SPACING),
        false
    );

    ImGui::Spacing();
    if (ImGui::BeginTable("ControlsTable", 2,
        ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV))
    {
        // ================== TECLAT ==================
        ImGui::TableNextColumn();

        CenteredColumnTitle(u8"Teclat");
        ImGui::Separator();

        ImGui::TextUnformatted(u8"Conducció:");
        BulletLabel(u8"W / S:", u8" Accelerar / Fre i Marxa Enrere");
        BulletLabel(u8"A / D:", u8" Girar volant (Esquerra / Dreta)");
        BulletLabel(u8"Espai:", u8" Fre de peu (Principal)");
        BulletLabel(u8"P:", u8" Fre de mà (Activar / Desactivar)");
        BulletLabel(u8"G:", u8" Activar / Desactivar ABS");

        ImGui::Spacing();

        ImGui::TextUnformatted(u8"Llums i intermitents:");
        BulletLabel(u8"L:", u8" Llums (Apagades -> Curtes -> Llargues)");
        BulletLabel(u8"J / K:", u8" Intermitent esquerre / dret");
        BulletLabel(u8"H:", u8" Llums d'emergència (Warnings)");

        ImGui::Spacing();

        ImGui::TextUnformatted(u8"Càmera i sistema:");
        BulletLabel(u8"Fletxes esquerra / dreta:",
            u8" Rotar càmera al voltant del cotxe");
        BulletLabel(u8"Fletxa amunt:",
            u8" Recentrar càmera darrere del cotxe");
        BulletLabel(u8"C:",
            u8" Canviar vista (Interior / Exterior)");
        BulletLabel(u8"R:",
            u8" Reiniciar posició del cotxe (Respawn)");

        // ================== MANDO ==================
        ImGui::TableNextColumn();

        CenteredColumnTitle(u8"Mando (DualSense)");
        ImGui::Separator();

        ImGui::TextUnformatted(u8"Conducció:");
        BulletLabel(u8"R2:", u8" Accelerar");
        BulletLabel(u8"L2:", u8" Fre suau / Marxa enrere");
        BulletLabel(u8"Joystick esquerre:", u8" Direcció");
        BulletLabel(u8"Quadrat:", u8" Fre de peu");
        BulletLabel(u8"Creu dreta:", u8" Fre de mà");
        BulletLabel(u8"Creu amunt:", u8" Activar / Desactivar ABS");

        ImGui::Spacing();

        ImGui::TextUnformatted(u8"Llums i intermitents:");
        BulletLabel(u8"Triangle:",
            u8" Canviar llums (Apagades -> Curtes -> Llargues)");
        BulletLabel(u8"L1 / R1:",
            u8" Intermitent esquerre / dret");
        BulletLabel(u8"Cercle:",
            u8" Llums d'emergència");

        ImGui::Spacing();

        ImGui::TextUnformatted(u8"Càmera i sistema:");
        BulletLabel(u8"Joystick dret:", u8" Rotar càmera");
        BulletLabel(u8"Panell tàctil:", u8" Recentrar càmera");
        BulletLabel(u8"Creu avall:",
            u8" Canviar vista (Interior / Exterior)");
        BulletLabel(u8"Creu esquerra:",
            u8" Reiniciar posició del cotxe (Respawn)");

        ImGui::EndTable();
    }



    ImGui::EndChild(); 
    ImGui::SetCursorPosX(centered_x_pos);
    if (ImGui::Button(u8"Enrere", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT))) {
        if(controller.n == 1)   
            controller.SwitchState(new MainMenuState());
        else 
            controller.SwitchState(new PauseMenuState());
    }

    controller.PopUserNeonStyle();
    ImGui::End();
}
