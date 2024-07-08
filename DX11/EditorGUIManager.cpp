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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(Application::GetI()->GetMainHwnd());
    ImGui_ImplDX11_Init(Application::GetI()->GetDevice(), Application::GetI()->GetDeviceContext());

    // Load Fonts
    //io.Fonts->AddFontDefault(); 
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesKorean());
    io.Fonts->Build();
}

void EditorGUIManager::Update()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 상단에 플레이, 스탑, 한 프레임씩 넘기기 버튼 추가
    ImGui::BeginMainMenuBar();
    if (ImGui::Button("Play"))
    {
        Application::SetPlaying(true);
        SceneManager::GetI()->HandlePlay();
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
    {
        Application::SetPlaying(false);
        SelectionManager::ClearSelection(); 
        SceneManager::GetI()->HandleStop(); 
    }
    ImGui::SameLine();
    if (ImGui::Button("Step"))
    {
    }
    ImGui::EndMainMenuBar();

    // 도킹 가능한 영역 설정
    ImGuiViewport* viewport = ImGui::GetMainViewport(); 
    ImGui::SetNextWindowPos(viewport->Pos); 
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking; 
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus; 
     
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f); 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", NULL, window_flags); 
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MyDockspace"); 
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None); 
    ImGui::End(); 

    for (auto& window : m_pEditorWindows)
    {
        window->Update();
    }
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
