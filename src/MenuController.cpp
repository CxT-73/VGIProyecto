#include "MenuController.h"

MenuController::MenuController(GameContext* data, char* cameraPtr, Coche* coche)
    : currentState(nullptr), contextData(data), externalCameraPtr(cameraPtr), miCoche(coche){
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
float MenuController::BeginButtonWindow(const char* name,
    float required_content_height,
    float forced_width) {
    ImVec2 display_size = ImGui::GetIO().DisplaySize;

    ImGuiWindowFlags menu_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBackground;
    // ? Quitamos AlwaysAutoResize

    ImVec2 menu_size(forced_width, required_content_height + MARGIN);

    ImGui::SetNextWindowSize(menu_size);
    ImGui::SetNextWindowPos(
        ImVec2(
            (display_size.x - menu_size.x) * 0.5f,
            (display_size.y - menu_size.y) * 0.5f
        ),
        ImGuiCond_Always
    );

    ImGui::Begin(name, nullptr, menu_flags);

    float centered_x_pos = (ImGui::GetWindowWidth() - BUTTON_WIDTH) * 0.5f;
    float centered_y_offset =
        (ImGui::GetWindowHeight() - required_content_height) * 0.5f;

    ImGui::SetCursorPosY(centered_y_offset);
    ImGui::SetCursorPosX(centered_x_pos);

    return centered_x_pos;
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

    // 1. REGLA DE ORO: SI MUERE (VIDA <= 0), PUNTUACIÓN AUTOMÁTICA 0
    if (contextData->carHealth <= 0) {
        contextData->score = 0;
        return;
    }

    float notaFinal = 10.0f; // Empezamos en la nota máxima

    // --- CONFIGURACIÓN DE LÍMITES ---
    const float TIEMPO_PERFECTO = 240.0f;      // 4 minutos
    const float TIEMPO_MAXIMO_ADMITIDO = 600.0f; // 10 minutos
    const float PENALIZACION_CHOQUE = 1.0f;    // -1 punto por choque

    float timeTaken = (contextData->finalTime > 0.0f) ? contextData->finalTime : contextData->gameTime;

    // 2. PENALIZACIÓN POR TIEMPO
    if (timeTaken > TIEMPO_PERFECTO) {
        // Calculamos cuánto se ha pasado de los 4 minutos
        float exceso = timeTaken - TIEMPO_PERFECTO;
        float margen = TIEMPO_MAXIMO_ADMITIDO - TIEMPO_PERFECTO;

        // Si tarda 10 min o más, la penalización máxima por tiempo es de 5 puntos
        float penalizacionTiempo = (exceso / margen) * 5.0f;
        notaFinal -= penalizacionTiempo;
    }

    // 3. PENALIZACIÓN POR COLISIONES
    notaFinal -= (contextData->collisionCount * PENALIZACION_CHOQUE);

    // 4. REGLA DE LOS 5 CHOQUES (CAP)
    if (contextData->collisionCount > 5) {
        if (notaFinal > 5.0f) {
            notaFinal = 5.0f; // Máximo aspiras a 5
        }
    }

    // 5. AJUSTE FINAL Y GUARDADO
    if (notaFinal < 0.0f) notaFinal = 0.0f;
    if (notaFinal > 10.0f) notaFinal = 10.0f;

    contextData->score = static_cast<int>(notaFinal);
}