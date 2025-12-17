#include "MenuController.h"
#include "Credits.h"
#include "MainMenu.h"
#include "PauseMenu.h"

void CreditsState::Render(MenuController& controller)
{
    controller.setState("Credits");
    controller.DrawBackgroundOverlay();

    float total_content_height = 900.0f;

    float centered_x_pos =
        controller.BeginButtonWindow("CreditsWindow", total_content_height, 1000.0f);

    controller.PushUserNeonStyle();
     
    ImGui::SetWindowFontScale(2.0f);
    const char* title = u8"CRÈDITS";
    float title_width = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - title_width) * 0.5f);
    ImGui::TextColored(NEON_CYAN_TITLE, title);
    ImGui::SetWindowFontScale(1.0f);

    ImGui::Spacing();
    ImGui::Spacing();
     
    ImGui::BeginChild("CreditsScroll", ImVec2(0, -BUTTON_HEIGHT - SPACING), false);

    auto SectionTitle = [&](const char* text)
        {
            ImGui::Spacing();
            ImGui::TextColored(NEON_CYAN_TITLE, "%s", text);
            ImGui::Separator();
        };

    // PROJECTE
    SectionTitle(u8"Projecte");
    ImGui::TextUnformatted(u8"GIMCANA AUTOMOBILÍSTICA");
    ImGui::TextUnformatted(u8"Visualització Gràfica Interactiva");

    // EQUIP
    SectionTitle(u8"Equip de desenvolupament");
    ImGui::BulletText(u8"Jose Angel Boza Zapater");
    ImGui::BulletText(u8"Alvaro Ruiz Hi");
    ImGui::BulletText(u8"Joana Figueroa Zapata");
    ImGui::BulletText(u8"Oussama Berrouhou");
    ImGui::BulletText(u8"Iker Bolancel Fernández");
    ImGui::BulletText(u8"Pau Pérez Cardús");

    // PROGRAMACIÓ
    SectionTitle(u8"Programació i sistemes");
    ImGui::TextUnformatted(u8"Moviment i col·lisions:");
    ImGui::BulletText(u8"Oussama Berrouhou");
    ImGui::BulletText(u8"Pau Pérez Cardús");

    ImGui::TextUnformatted(u8"Càmeres:");
    ImGui::BulletText(u8"Iker Bolancel Fernández");

    ImGui::TextUnformatted(u8"Adaptació al comandament:");
    ImGui::BulletText(u8"Alvaro Ruiz Hi");

    // ESCENA I GRÀFICS
    SectionTitle(u8"Disseny d'escena i gràfics");

    ImGui::TextUnformatted(u8"Col·locació d'objectes:");
    ImGui::BulletText(u8"Joana Figueroa Zapata");

    ImGui::TextUnformatted(u8"Il·luminació:");
    ImGui::BulletText(u8"Iker Bolancel Fernández");
    ImGui::BulletText(u8"Pau Pérez Cardús");
    ImGui::BulletText(u8"Alvaro Ruiz Hi");

    ImGui::TextUnformatted(u8"Texturització:");
    ImGui::BulletText(u8"Alvaro Ruiz Hi");
    ImGui::BulletText(u8"Joana Figueroa Zapata");

    ImGui::TextUnformatted(u8"Modelatge 3D:");
    ImGui::BulletText(u8"Alvaro Ruiz Hi");
    ImGui::BulletText(u8"Pau Pérez Cardús");
    ImGui::BulletText(u8"Joana Figueroa Zapata");

    // UI
    SectionTitle(u8"Interfície i experiència d'usuari");
    ImGui::TextUnformatted(u8"Menús i interfície:");
    ImGui::BulletText(u8"Oussama Berrouhou");
    ImGui::BulletText(u8"Iker Bolancel Fernández");
    ImGui::BulletText(u8"Jose Angel Boza Zapater");
    ImGui::BulletText(u8"Joana Figueroa Zapata");

    // TESTING
    SectionTitle(u8"Proves i ajustos");
    ImGui::TextUnformatted(u8"Testing i ajustos de jugabilitat:");
    ImGui::BulletText(u8"Jose Angel Boza Zapater");
    ImGui::BulletText(u8"Alvaro Ruiz Hi");
    ImGui::BulletText(u8"Joana Figueroa Zapata");
    ImGui::BulletText(u8"Oussama Berrouhou");
    ImGui::BulletText(u8"Iker Bolancel Fernández");
    ImGui::BulletText(u8"Pau Pérez Cardús");

    // TECNOLOGIES
    SectionTitle(u8"Tecnologies utilitzades");
    ImGui::TextUnformatted(u8"OpenGL · Bullet Physics · GLFW · ImGui · GLM");
    ImGui::TextUnformatted(u8"Visual Studio · Blender · Git / GitHub");

    // CONTEXT ACADÈMIC
    SectionTitle(u8"Context acadèmic");
    ImGui::TextUnformatted(u8"Grau en Enginyeria Informàtica");
    ImGui::TextUnformatted(u8"Aprenentatge Basat en Projectes (ABP)");
    ImGui::TextUnformatted(u8"Universitat Autònoma de Barcelona");

    ImGui::EndChild();
     
    ImGui::Spacing();

    float button_x = (ImGui::GetWindowWidth() - BUTTON_WIDTH) * 0.5f;
    ImGui::SetCursorPosX(button_x);

    if (ImGui::Button(u8"Enrere", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT)))
    {
        if (controller.n == 1)
            controller.SwitchState(new MainMenuState());
        else
            controller.SwitchState(new PauseMenuState());
    }

    controller.PopUserNeonStyle();
    ImGui::End();
}
