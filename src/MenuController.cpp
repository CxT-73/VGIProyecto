#include "MenuController.h"

MenuController::MenuController(GameContext* data, char* cameraPtr)
    : currentState(nullptr), contextData(data), externalCameraPtr(cameraPtr) {
}

MenuController::~MenuController() {
    if (currentState) delete currentState;
}

void MenuController::SwitchState(IMenuState* newState) {
    if (currentState) {
        delete currentState;
    }
    currentState = newState;
}

void MenuController::Render() {
    if (currentState) {
        currentState->Render(*this);
    }
}

GameContext* MenuController::GetContext() { return contextData; }

void MenuController::DrawBackgroundOverlay() {
    ImGuiWindowFlags background_flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    // Color de fondo (0.1f, 0.1f, 0.15f, 0.64f)
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.15f, 0.64f));

    if (ImGui::Begin("GlobalOverlayBackground", nullptr, background_flags)) {}
    ImGui::End();
    ImGui::PopStyleColor(); // Pop ImGuiCol_WindowBg
}

// --- 2. AYUDANTE PARA CENTRADO (Calcula y prepara la ventana invisible) ---
// Devuelve el offset Y necesario para centrar el contenido, y posiciona el cursor X.
float MenuController::BeginButtonWindow(const char* name, float required_content_height) {
    ImVec2 display_size = ImGui::GetIO().DisplaySize;

    ImGuiWindowFlags menu_flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize;

    ImVec2 menu_size(BUTTON_WIDTH + MARGIN, required_content_height + MARGIN);
    ImGui::SetNextWindowSize(menu_size);
    ImGui::SetNextWindowPos(ImVec2(
        (display_size.x - menu_size.x) * 0.5f,
        (display_size.y - menu_size.y) * 0.5f
    ));

    ImGui::Begin(name, nullptr, menu_flags);

    // Centrado de los botones dentro de esta ventana
    float centered_x_pos = (ImGui::GetWindowWidth() - BUTTON_WIDTH) * 0.5f;
    float centered_y_offset = (ImGui::GetWindowHeight() - required_content_height) * 0.5f;

    // Posicionar el cursor Y
    ImGui::SetCursorPosY(centered_y_offset);

    // Posicionar el cursor X para que los elementos empiecen centrados
    ImGui::SetCursorPosX(centered_x_pos);

    return centered_x_pos; // Devolvemos la posición X para que las llamadas la usen
}

// --- 3. APLICAR ESTILO NEÓN LOCAL (Según tu código de prueba) ---
void MenuController::PushUserNeonStyle() {
    // 5 Colores (Button, Hovered, Active, Text, Border)
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.6f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Texto Blanco
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.7f, 0.9f, 0.5f));

    // --- CLAVE 1: FONDO OSCURO PARA EL DESPLEGABLE (PopupBg) ---
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.1f, 0.1f, 0.15f, 0.9f));

    // 2 Variables (FrameRounding, FrameBorderSize)
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

    // 3 Colores de Frame
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.15f, 0.15f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.0f, 0.6f, 0.8f, 1.0f));
}

void MenuController::PopUserNeonStyle() {
    ImGui::PopStyleColor(9); // 5 (Base) + 1 (PopupBg) + 3 (Frames) = 9
    ImGui::PopStyleVar(2);
}

void MenuController::calculateScore() {
    if (!contextData) return;

    // --- CONFIGURACIÓN DE PUNTUACIÓN ---
    const float TIME_LIMIT = 300.0f;      // Tiempo objetivo (ej: 5 minutos / 300 segundos)
    const int POINTS_PER_HEALTH = 100;    // Puntos por cada 1% de vida
    const int POINTS_PER_SEC_SAVED = 50;  // Puntos por cada segundo ahorrado respecto al límite
    const int PENALTY_PER_COLLISION = 200;// Puntos restados por cada choque

    // 1. Puntuación por Vida Restante
    // GameContext::carHealth
    int healthScore = contextData->carHealth * POINTS_PER_HEALTH;

    // 2. Puntuación por Tiempo
    // Usamos finalTime si la carrera terminó, de lo contrario usamos gameTime actual
    float timeTaken = (contextData->finalTime > 0.0f) ? contextData->finalTime : contextData->gameTime;

    float timeSaved = TIME_LIMIT - timeTaken;
    if (timeSaved < 0.0f) timeSaved = 0.0f; // No damos puntos negativos por tiempo, solo 0 bonus

    int timeScore = static_cast<int>(timeSaved * POINTS_PER_SEC_SAVED);

    // 3. Penalización por Colisiones
    // GameContext::collisionCount
    int collisionPenalty = contextData->collisionCount * PENALTY_PER_COLLISION;

    // 4. Cálculo Total
    int totalScore = (healthScore + timeScore) - collisionPenalty;

    // Evitamos que la puntuación sea negativa
    if (totalScore < 0) totalScore = 0;

    // 5. Guardar resultado en el contexto
    contextData->score = totalScore;
}