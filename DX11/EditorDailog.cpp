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
    // 창이 닫혔을 때의 상태를 감지
    if (m_PreviousWindowOpen && !m_WindowOpen)
    {
        Close(); 
    }

    // 창이 닫혀 있으면 렌더링하지 않음
    if (!m_WindowOpen)
        return;

    // 화면의 너비와 높이를 가져옵니다.
    ImGuiIO& io = ImGui::GetIO();

    float window_width = 400.0f;  // 창의 너비
    float window_height = 300.0f; // 창의 높이

    // 창의 크기를 설정합니다.
    ImGui::SetNextWindowSize(ImVec2(window_width, window_height), ImGuiCond_FirstUseEver);

    // 도킹을 비활성화하고 창의 테두리를 추가하기 위한 플래그 설정
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;
        
    EditorGUI::EditorWindowStylePush();

    // 스타일 설정 시작
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f); // 전체 창 라운딩 없앰
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f); // 창의 테두리 두께 설정

    // 타이틀바 색상 변경 (진한 회색)
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

    // 도킹된 탭의 라운딩 값을 설정
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 12.0f);  // 탭의 라운딩 값 설정
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 12.0f);

    // 도킹된 탭 바 배경색 설정
    ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));           // 일반 탭 색상
    ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));    // 호버된 탭 색상
    ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));     // 활성화된 탭 색상
    ImGui::PushStyleColor(ImGuiCol_TabUnfocused, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));  // 포커스되지 않은 탭 색상
    ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4(0.4f, 0.4f, 0.4f, 0.75f)); // 비활성 상태에서 선택된 탭 색상

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

    // 타이틀바 텍스트의 색상 설정
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // 텍스트 흰색

    // 윈도우를 시작합니다. 두 번째 인자로 창 열림 상태를 전달 
    if (ImGui::Begin(m_WindowName.c_str(), &m_WindowOpen, windowFlags)) 
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        OnRender(); // 주요 렌더링 함수 호출
        ImGui::PopStyleVar();
    }
    ImGui::End();

    // 스타일 설정 복구
    ImGui::PopStyleColor(3);  // TitleBg, TitleBgActive, TitleBgCollapsed, Text 복구
    ImGui::PopStyleVar(2);    // FrameRounding, WindowRounding 복구

    // 스타일 설정 복구
    ImGui::PopStyleColor(4);    // Tab, TabHovered, TabActive, TabUnfocused 복구
    ImGui::PopStyleColor(6);    // Tab, TabHovered, TabActive, TabUnfocused 복구
    ImGui::PopStyleVar(2);      // TabRounding 복구

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
