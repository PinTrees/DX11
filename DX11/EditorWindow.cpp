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
    // 화면의 너비와 높이를 가져옵니다.
    ImGuiIO& io = ImGui::GetIO();

    float window_width = 400.0f;  // 창의 너비
    float window_height = 300.0f; // 창의 높이

    // 창의 크기를 설정합니다.
    ImGui::SetNextWindowSize(ImVec2(window_width, window_height), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;

    EditorGUI::EditorWindowStylePush();

    // 스타일 설정 시작
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f); // 전체 창 라운딩 없앰

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

    // 윈도우를 시작합니다
    if (ImGui::Begin(windowName.c_str(), nullptr, windowFlags))
    {
        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("ContextMenu");
        }

        // 컨텍스트 메뉴 렌더링
        if (ImGui::BeginPopup("ContextMenu"))
        {
            if (ImGui::MenuItem("Add Tab"))
            {
            }
            ImGui::EndPopup();
        }

        OnRender(); // 주요 렌더링 함수 호출
    }
    ImGui::End();

    // 스타일 설정 복구
    ImGui::PopStyleColor(3);  // TitleBg, TitleBgActive, TitleBgCollapsed, Text 복구
    ImGui::PopStyleVar(1);    // FrameRounding, WindowRounding 복구

    // 스타일 설정 복구
    ImGui::PopStyleColor(4);  // Tab, TabHovered, TabActive, TabUnfocused 복구
    ImGui::PopStyleColor(6);  // Tab, TabHovered, TabActive, TabUnfocused 복구
    ImGui::PopStyleVar(2);      // TabRounding 복구

    EditorGUI::EditorWindowStylePop();
}

