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
    // ���� ��θ� ��ܿ� ǥ��
    ImGui::Text("Current Directory: %s", currentDirectory.c_str());

    // ���� ���� Ʈ�� ��
    ImGui::BeginChild("FolderTree", ImVec2(250, 0), true);
    DisplayDirectoryTree(solutionDirectory);
    ImGui::EndChild();

    ImGui::SameLine();

    // ���� ���� ���丮�� ���� �� ���� ��
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

                // �巡�� �� ��� �ҽ� ����
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

    // �ַ�� ��θ� ã�� ���� �θ� ���丮�� Ž���մϴ�.
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