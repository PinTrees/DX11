#include "pch.h"
#include "EditorGUIManager.h"

#include "EditorWindow.h"
#include "EditorGUI.h"
#include "App.h"

SINGLE_BODY(EditorGUIManager)

EditorGUIManager::EditorGUIManager()
    : m_IsInit(false)
{

}

EditorGUIManager::~EditorGUIManager()
{
    Safe_Delete_Vec(m_pEditorWindows);
    Safe_Delete_Vec(m_pEditorDialogs); 
}

void EditorGUIManager::Init()
{
    if (m_IsInit)
        return;

    m_IsInit = true; 

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; 
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts; 

    io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines;
    io.FontGlobalScale = 1.0f;
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f); 

    ImGui::StyleColorsDark(); 
    SetStyle_Base();  

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_EnableDpiAwareness();  
    ImGui_ImplWin32_Init(Application::GetI()->GetMainHwnd());
    ImGui_ImplDX11_Init(Application::GetI()->GetDevice(), Application::GetI()->GetDeviceContext());

    float fontSize = 24.0f;

    ImFontConfig config;
    config.MergeMode = true; // 기존 폰트와 합쳐 사용 
    config.PixelSnapH = true;
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 }; // FontAwesome 범위   
    string fa_path = PathManager::GetI()->GetContentPathS() + "ProjectSetting\\fonts\\fa-solid-900.ttf";
    
    // Load Fonts
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", fontSize, NULL, io.Fonts->GetGlyphRangesKorean());
    io.Fonts->AddFontFromFileTTF(fa_path.c_str(), fontSize - 4, &config, icons_ranges); 
    io.Fonts->Build();
}

void EditorGUIManager::Destroy() 
{
    ImGui_ImplDX11_Shutdown();  
    ImGui_ImplWin32_Shutdown();  
    ImGui::DestroyContext();  
}

void EditorGUIManager::Update()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    for (auto& window : m_pEditorWindows)
    {
        window->Update();
    }
}

void EditorGUIManager::RenderEditorWindows()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(18.0f, 18.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.07f, 0.07f, 0.07f, 1.0f));

    if (ImGui::BeginMainMenuBar())
    {
        // 메뉴바의 실제 높이를 계산합니다.
        float menuBarHeight = ImGui::GetFrameHeight();
    
        float imageHeight = 12;
        float buttonPaddingX = 20;
        float buttonPaddingY = 11;
        float buttonHeight = imageHeight + buttonPaddingY * 2;
        float centerOffset = (menuBarHeight - buttonHeight) * 0.5f;
    
        EditorTextStyle textStyle;
        textStyle.FontSize = 22;
        textStyle.Bold = true;
        ImGui::SetCursorPosY(-6);
        EditorGUI::Label("Unity Imitation 0.0.1", textStyle);

        // Play
        ImGui::Dummy(ImVec2(24, 0));
        ImGui::SetCursorPosY(centerOffset);
        if (EditorGUI::ImageButton(L"\\ProjectSetting\\icons\\icon_editor_play.png", ImVec2(imageHeight, imageHeight), ImVec2(buttonPaddingX, buttonPaddingY)))
        {
            Application::SetPlaying(true);
            SceneManager::GetI()->HandlePlay();
        }
        // Stop
        ImGui::Dummy(ImVec2(2, 0));
        ImGui::SetCursorPosY(centerOffset);
        if (EditorGUI::ImageButton(L"\\ProjectSetting\\icons\\icon_editor_stop.png", ImVec2(imageHeight, imageHeight), ImVec2(buttonPaddingX, buttonPaddingY)))
        {
            Application::SetPlaying(false);
            SelectionManager::ClearSelection();
            SceneManager::GetI()->HandleStop();
        }
        // Step
        ImGui::Dummy(ImVec2(2, 0));
        ImGui::SetCursorPosY(centerOffset);
        if (EditorGUI::ImageButton(L"\\ProjectSetting\\icons\\icon_editor_stop.png", ImVec2(imageHeight, imageHeight), ImVec2(buttonPaddingX, buttonPaddingY)))
        {
            // Step 기능
        }
    }
    // 메뉴바 높이 및 색상 스타일 복원
    ImGui::EndMainMenuBar();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(4);

    // 창의 전체 크기와 위치 설정
    // 프레임과 배경을 제거하는 플래그 설정
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoBackground;

    // 전체 화면 크기로 DockSpace 창 설정    
    Vec2 screenSize = Application::GetI()->GetApp()->GetScreenSize();
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + 48)); 
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - 48)); 
    ImGui::SetNextWindowViewport(viewport->ID);  

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f); 
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

    if (ImGui::Begin("##DockSpace", NULL, window_flags))
    {
        ImGuiID dockspace_id = ImGui::GetID("RootDockspace");  
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None); 
    }

    ImGui::End();

    ImGui::PopStyleVar(5);

    for (auto& window : m_pEditorWindows)
    {
        window->Render();
    }
    
    for (auto& dialog : m_pEditorDialogs) 
    { 
        dialog->Render(); 
    } 
}

void EditorGUIManager::RenderAfter()
{
    ImGuiIO& io = ImGui::GetIO(); 
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void EditorGUIManager::OnResize(Vec2 size)
{
    if (!m_IsInit)
        return;

    //ImGuiIO& io = ImGui::GetIO(); 
    //io.DisplaySize = ImVec2(size.x, size.y);
}

void EditorGUIManager::RegisterWindow(EditorWindow* window)
{
    m_pEditorWindows.push_back(window);
}

void EditorGUIManager::RegisterEditorDialog(EditorDialog* dialog)
{
    m_pEditorDialogs.push_back(dialog); 
}

void EditorGUIManager::RemoveEditorDialog(EditorDialog* dialog)
{
    auto it = std::remove(m_pEditorDialogs.begin(), m_pEditorDialogs.end(), dialog);

    if (it != m_pEditorDialogs.end())
    {
        m_pEditorDialogs.erase(it, m_pEditorDialogs.end());
    }
}


void EditorGUIManager::SetStyle_Base()
{
    ImGuiStyle* style = &ImGui::GetStyle(); 
    
    style->DockingSeparatorSize = 4.0f;
    
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);

    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    
    colors[ImGuiCol_Border] = ImVec4(0.07, 0.07f, 0.07f, 1.0f);         // 도킹 디바이드 기본 컬러
    colors[ImGuiCol_BorderShadow] = ImVec4(0.07f, 0.07f, 0.07f, 0.0f);  // 도킹 디바이드 기본 컬러

    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);          // 도킹 탭 기본 배경
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);    // 도킹 탭 타이틀 액티브 배경
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.07f, 1.0f);  // 도킹 탭 리스트 배경 
    
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

    colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);

    colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    
    colors[ImGuiCol_Separator]          = ImVec4(0.07f, 0.07f, 0.07f, 1.0f);
    colors[ImGuiCol_SeparatorHovered]   = ImVec4(0.07f, 0.07f, 0.07f, 1.f);
    colors[ImGuiCol_SeparatorActive]    = ImVec4(0.07f, 0.07f, 0.07f, 1.f);

    colors[ImGuiCol_ResizeGrip]         = ImVec4(0.26f, 0.59f, 0.98f, 0.0f);    // 우측 하단 크기조절 가능 힌트
    colors[ImGuiCol_ResizeGripHovered]  = ImVec4(0.26f, 0.59f, 0.98f, 0.0f);
    colors[ImGuiCol_ResizeGripActive]   = ImVec4(0.26f, 0.59f, 0.98f, 0.0f);

    colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_Tab] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); 
    colors[ImGuiCol_TabSelected] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    colors[ImGuiCol_TabDimmed] = colors[ImGuiCol_Tab];
    colors[ImGuiCol_TabDimmedSelected] = colors[ImGuiCol_TabSelected];
    colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);

    colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 0.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 0.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 0.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 0.00f);   // Prefer using Alpha=1.0 here

    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.0f);
    colors[ImGuiCol_TextLink] = colors[ImGuiCol_HeaderActive];
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    
    //colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);                // 도킹 디바이드 바 액티브 색상
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);    // 
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);        
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}