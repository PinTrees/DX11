#include "pch.h"
#include "ProjectEditorWindow.h"
#include <filesystem>

namespace fs = std::filesystem;

ProjectEditorWindow::ProjectEditorWindow()
	: EditorWindow("Project"),
	currentDirectory(".")
{
    solutionDirectory = GetSolutionDirectory();
    currentDirectory = solutionDirectory;
}

ProjectEditorWindow::~ProjectEditorWindow()
{
}

void ProjectEditorWindow::OnRender()
{
    // 현재 경로를 상단에 표시
    ImGui::Text("Current Directory: %s", currentDirectory.c_str());

    // 좌측 폴더 트리 뷰
    ImGui::BeginChild("FolderTree", ImVec2(250, 0), true);
    DisplayDirectoryTree(solutionDirectory);
    ImGui::EndChild();

    ImGui::SameLine();

    // 우측 현재 디렉토리의 파일 및 폴더 뷰
    ImGui::BeginChild("DirectoryContents", ImVec2(0, 0), true);
    DisplayDirectoryContents(currentDirectory);
    ImGui::EndChild();
}

void ProjectEditorWindow::DisplayDirectoryTree(const std::string& directory)
{
    for (const auto& entry : fs::directory_iterator(directory))
    {
        if (entry.is_directory())
        {
            if (ImGui::TreeNode(entry.path().filename().string().c_str()))
            {
                if (ImGui::IsItemClicked())
                {
                    currentDirectory = entry.path().string();
                }
                DisplayDirectoryTree(entry.path().string());
                ImGui::TreePop();
            }
        }
    }
}

void ProjectEditorWindow::DisplayDirectoryContents(const std::string& directory)
{
    for (const auto& entry : fs::directory_iterator(directory))
    {
        if (entry.is_directory())
        {
            if (ImGui::Selectable(entry.path().filename().string().c_str()))
            {
                currentDirectory = entry.path().string();
            }
        }
        else
        {
            if (entry.path().extension() == ".fbx")
            {
                if (ImGui::Selectable(entry.path().filename().string().c_str()))
                {
                    currentDirectory = entry.path().parent_path().string();
                }

                // 드래그 앤 드롭 소스 시작
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    std::string filePath = entry.path().string();
                    ImGui::SetDragDropPayload("FBX_FILE", &filePath, sizeof(std::string));
                    ImGui::Text("Dragging %s", entry.path().filename().string().c_str());
                    ImGui::EndDragDropSource();
                }
            }
            else
            {
                ImGui::Text(entry.path().filename().string().c_str());
            }
        }
    }
}

std::string ProjectEditorWindow::GetSolutionDirectory()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    std::string executablePath = std::string(buffer).substr(0, pos);

    // 솔루션 경로를 찾기 위해 부모 디렉토리를 탐색합니다.
    fs::path currentPath = executablePath;
    while (!currentPath.empty())
    {
        for (const auto& entry : fs::directory_iterator(currentPath))
        {
            if (entry.path().extension() == ".sln")
            {
                return currentPath.string();
            }
        }
        currentPath = currentPath.parent_path();
    }

    return executablePath;
}