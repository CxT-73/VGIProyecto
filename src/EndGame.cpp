// EndGame.cpp

#include "EndGame.h"
#include "MainMenu.h"
#include <cstdio> // Para snprintf

// Define un color de acento adicional, por ejemplo, un verde neón
const ImVec4 NEON_GREEN_TITLE = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

void EndGameState::Render(MenuController& controller) {

    char* cameraPtr = controller.GetCameraPtr();
    if (cameraPtr) {
        // Asumiendo que 'I' es CAM_INICI (Cámara de inicio/menú)
        *cameraPtr = 'I'; 
    }

    controller.DrawBackgroundOverlay();
    controller.setState("NewGame");
    // ----------------------------------------------------------------------
    // CÁLCULO DE ALTURA REQUERIDA (Ajustado para títulos y puntuación grandes)
    // ----------------------------------------------------------------------
    float base_lh = ImGui::GetTextLineHeight();
    float total_content_height = (3.0f * base_lh) + // Título grande
                                 (3.0f * base_lh) + (3.0f * SPACING) + // Stats
                                 (3.0f * base_lh) + // Score resaltado
                                 BUTTON_HEIGHT + (3.0f * SPACING); // Botón y espaciado

    float centered_x_pos = controller.BeginButtonWindow("EndGameButtons", total_content_height);

    controller.PushUserNeonStyle();
    
    // ======================================================================
    // 1. TÍTULO DE IMPACTO (Centrado y Grande)
    // ======================================================================
    const char* title = "PRACTICA FINALIZADA";
    ImGui::SetWindowFontScale(1.0f); 
    float title_width = ImGui::CalcTextSize(title).x;

    // Centrar el título dentro del área de la ventana
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - title_width) * 0.5f);
    ImGui::TextColored(NEON_GREEN_TITLE, title); // Usamos verde para el éxito
    ImGui::SetWindowFontScale(1.0f); 
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();
    
    // ======================================================================
    // 2. ESTADÍSTICAS ALINEADAS
    // ======================================================================

    // Movemos el cursor para que las estadísticas se vean centradas en la ventana
    // Usaremos el ancho del botón como referencia de centrado.
    const float STATS_WIDTH = 350.0f; 
    ImGui::SetCursorPosX(centered_x_pos + (BUTTON_WIDTH - STATS_WIDTH) * 0.5f);
    
    // Etiqueta (Blanca) y Valor (Por defecto)
    ImGui::Text("TIEMPO:"); ImGui::SameLine(centered_x_pos + STATS_WIDTH * 0.5f);
    ImGui::TextColored(NEON_CYAN_TITLE, "%.2f s", controller.GetContext()->finalTime);
    ImGui::Spacing();
    
    ImGui::SetCursorPosX(centered_x_pos + (BUTTON_WIDTH - STATS_WIDTH) * 0.5f);
    ImGui::Text("COLISIONES:"); ImGui::SameLine(centered_x_pos + STATS_WIDTH * 0.5f);
    ImGui::TextColored(NEON_CYAN_TITLE, "%d", controller.GetContext()->collisionCount);
    ImGui::Spacing(); ImGui::Spacing();
    
    // ======================================================================
    // 3. PUNTUACIÓN RESALTADA Y CENTRADA
    // ======================================================================
    
    const char* score_label = "PUNTUACION FINAL:";
    char score_buffer[64];
    snprintf(score_buffer, sizeof(score_buffer), "%d pts", controller.GetContext()->score);

    ImGui::SetWindowFontScale(1.0f); 
    
    ImVec2 score_label_size = ImGui::CalcTextSize(score_label);
    ImVec2 score_value_size = ImGui::CalcTextSize(score_buffer);
    
    float score_line_width = score_label_size.x + score_value_size.x + 20.0f; // Espacio entre etiqueta y valor
    
    // Centrar la línea completa
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - score_line_width) * 0.5f);

    // Etiqueta
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", score_label); 
    ImGui::SameLine(); 
    
    // Valor (Color Neón)
    ImGui::TextColored(NEON_CYAN_TITLE, "%s", score_buffer); 

    ImGui::SetWindowFontScale(1.0f); 
    ImGui::Spacing(); ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); 
    
    // ======================================================================
    // 4. BOTÓN
    // ======================================================================

    ImGui::SetCursorPosX(centered_x_pos); 
    ImVec2 button_size(BUTTON_WIDTH, BUTTON_HEIGHT);

    if (ImGui::Button("Volver al Menu Principal", button_size)) {
        // Lógica de salida
        controller.GetContext()->isGameRunning = false;
        controller.SwitchState(new MainMenuState());
    }

    controller.PopUserNeonStyle(); 
    ImGui::End();
}