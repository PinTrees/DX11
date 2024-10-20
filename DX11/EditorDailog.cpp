#include "pch.h"
#include "EditorDailog.h"
#include "EditorGUI.h"
#include "EditorGUIManager.h"

EditorDialog::EditorDialog(const std::string& name)
    : m_WindowName(name)
{
}

EditorDialog::~EditorDialog()
{
}

void EditorDialog::Render()
{
    // â�� ������ ���� ���¸� ����
    if (m_PreviousWindowOpen && !m_WindowOpen)
    {
        Close(); 
    }

    // â�� ���� ������ ���������� ����
    if (!m_WindowOpen)
        return;

    // ȭ���� �ʺ�� ���̸� �����ɴϴ�.
    ImGuiIO& io = ImGui::GetIO();

    float window_width = 400.0f;  // â�� �ʺ�
    float window_height = 300.0f; // â�� ����

    // â�� ũ�⸦ �����մϴ�.
    ImGui::SetNextWindowSize(ImVec2(window_width, window_height), ImGuiCond_FirstUseEver);

    // ��ŷ�� ��Ȱ��ȭ�ϰ� â�� �׵θ��� �߰��ϱ� ���� �÷��� ����
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;
        
    EditorGUI::EditorWindowStylePush();

    // ��Ÿ�� ���� ����
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f); // ��ü â ���� ����
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f); // â�� �׵θ� �β� ����

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

    // �����츦 �����մϴ�. �� ��° ���ڷ� â ���� ���¸� ���� 
    if (ImGui::Begin(m_WindowName.c_str(), &m_WindowOpen, windowFlags)) 
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        OnRender(); // �ֿ� ������ �Լ� ȣ��
        ImGui::PopStyleVar();
    }
    ImGui::End();

    // ��Ÿ�� ���� ����
    ImGui::PopStyleColor(3);  // TitleBg, TitleBgActive, TitleBgCollapsed, Text ����
    ImGui::PopStyleVar(2);    // FrameRounding, WindowRounding ����

    // ��Ÿ�� ���� ����
    ImGui::PopStyleColor(4);    // Tab, TabHovered, TabActive, TabUnfocused ����
    ImGui::PopStyleColor(6);    // Tab, TabHovered, TabActive, TabUnfocused ����
    ImGui::PopStyleVar(2);      // TabRounding ����

    EditorGUI::EditorWindowStylePop();
}

void EditorDialog::Show()
{
    m_WindowOpen = true;
    m_PreviousWindowOpen = true;

    EditorGUIManager::GetI()->RegisterEditorDialog(this);
}

void EditorDialog::Close()
{
    EditorGUIManager::GetI()->RemoveEditorDialog(this);  
}
