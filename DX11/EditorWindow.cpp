#include "pch.h"
#include "EditorWindow.h"
#include "EditorGUI.h"

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

    // â�� ũ�⸦ �����մϴ�.
    ImGui::SetNextWindowSize(ImVec2(window_width, window_height), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;

    EditorGUI::EditorWindowStylePush();

    // ��Ÿ�� ���� ����
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f); // ��ü â ���� ����

    // Ÿ��Ʋ�� ���� ���� (���� ȸ��)
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

    // ��ŷ�� ���� ���� ���� ����
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 12.0f);  // ���� ���� �� ����
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 12.0f);

    // ��ŷ�� �� �� ���� ����
    ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));           // �Ϲ� �� ����
    ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));    // ȣ���� �� ����
    ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));     // Ȱ��ȭ�� �� ����
    ImGui::PushStyleColor(ImGuiCol_TabUnfocused, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));  // ��Ŀ������ ���� �� ����
    ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4(0.4f, 0.4f, 0.4f, 0.75f)); // ��Ȱ�� ���¿��� ���õ� �� ����

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

    // Ÿ��Ʋ�� �ؽ�Ʈ�� ���� ����
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // �ؽ�Ʈ ���

    // �����츦 �����մϴ�
    if (ImGui::Begin(windowName.c_str(), nullptr, windowFlags))
    {
        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("ContextMenu");
        }

        // ���ؽ�Ʈ �޴� ������
        if (ImGui::BeginPopup("ContextMenu"))
        {
            if (ImGui::MenuItem("Add Tab"))
            {
            }
            ImGui::EndPopup();
        }

        OnRender(); // �ֿ� ������ �Լ� ȣ��
    }
    ImGui::End();

    // ��Ÿ�� ���� ����
    ImGui::PopStyleColor(3);  // TitleBg, TitleBgActive, TitleBgCollapsed, Text ����
    ImGui::PopStyleVar(1);    // FrameRounding, WindowRounding ����

    // ��Ÿ�� ���� ����
    ImGui::PopStyleColor(4);  // Tab, TabHovered, TabActive, TabUnfocused ����
    ImGui::PopStyleColor(6);  // Tab, TabHovered, TabActive, TabUnfocused ����
    ImGui::PopStyleVar(2);      // TabRounding ����

    EditorGUI::EditorWindowStylePop();
}

