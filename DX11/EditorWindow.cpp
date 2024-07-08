#include "pch.h"
#include "EditorWindow.h"


EditorWindow::EditorWindow(const std::string& name)
	: windowName(name),
	isDocked(false)
{
}

EditorWindow::~EditorWindow()
{
}

void EditorWindow::Render()
{
    // 화면의 너비와 높이를 가져옵니다.
    ImGuiIO& io = ImGui::GetIO();

    float window_width = 400.0f;  // 창의 너비
    float window_height = 300.0f; // 창의 높이

    // 창의 크기를 설정합니다.
    ImGui::SetNextWindowSize(ImVec2(window_width, window_height), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;

    PushStyle();

    ImGui::Begin(windowName.c_str(),  nullptr, windowFlags);

    OnRender();
    
    ImGui::End();

    OnRenderExit();

    PopStyle();
}

