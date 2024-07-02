#include "pch.h"
#include "EditorGUIManager.h"

#include "EditorWindow.h"

SINGLE_BODY(EditorGUIManager)

EditorGUIManager::EditorGUIManager()
{

}

EditorGUIManager::~EditorGUIManager()
{
    Safe_Delete_Vec(m_pEditorWindows);
}

void EditorGUIManager::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(Application::GetI()->GetMainHwnd());
    ImGui_ImplDX11_Init(Application::GetI()->GetDevice(), Application::GetI()->GetDeviceContext());

    // Load Fonts
    io.Fonts->AddFontDefault(); 
    io.Fonts->Build();
    // io.Fonts->AddFontFromFileTTF("path_to_font.ttf", 16.0f);
}

void EditorGUIManager::Update()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void EditorGUIManager::RenderEditorWindows()
{
    for (auto& window : m_pEditorWindows)
    {
        window->Render();
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void EditorGUIManager::RegisterWindow(EditorWindow* window)
{
    m_pEditorWindows.push_back(window);
}
