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
    // ȭ���� �ʺ�� ���̸� �����ɴϴ�.
    ImGuiIO& io = ImGui::GetIO();
    float window_width = 400.0f;  // â�� �ʺ�
    float window_height = 300.0f; // â�� ����

    // â�� ��ġ�� ȭ���� ���� ������� �����մϴ�.
    ImVec2 pos = ImVec2(io.DisplaySize.x - window_width, 0.0f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);

    // â�� ũ�⸦ �����մϴ�.
    ImGui::SetNextWindowSize(ImVec2(window_width, window_height), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;// | ImGuiWindowFlags_NoResize;
    ImGui::Begin(windowName.c_str(),  nullptr, windowFlags);

    OnRender();
    
    ImGui::End();

    OnRenderExit();
}

