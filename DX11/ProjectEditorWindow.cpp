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

    if (ImGui::IsItemClicked(1)) // Right-click
    {
        ImGui::OpenPopup("ItemContextMenu");
    }

    if (ImGui::BeginPopup("ItemContextMenu"))
    {
        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::MenuItem("Material"))
            {
                UMaterial::Create(currentDirectory);
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Folder"))
            {
                std::string folderPath = currentDirectory + "/New Folder";
                int folderIndex = 1;
                while (std::filesystem::exists(folderPath)) 
                {
                    folderPath = currentDirectory + "/New Folder (" + std::to_string(folderIndex++) + ")";
                }
                std::filesystem::create_directory(folderPath);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
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

void RenameSelectedFile(const fs::path& oldPath, const char* newName)
{
    fs::path newPath = oldPath.parent_path() / newName;
    try
    {
        fs::rename(oldPath, newPath);
        SelectionManager::SetSelectedFile(newPath.string());
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "Error renaming file: " << e.what() << std::endl;
    }
}

void ProjectEditorWindow::DisplayDirectoryContents(const std::string& directory)
{
    static char newName[256] = "";
    static bool renaming = false;
    static std::string renamingPath = "";

    for (const auto& entry : fs::directory_iterator(directory))
    {
        bool isSelected = (SelectionManager::GetSelectedFile() == entry.path().string());

        ImGui::PushID(entry.path().string().c_str());

        if (entry.is_directory())
        {
            if (ImGui::Selectable(entry.path().filename().string().c_str(), isSelected))
            {
                if (renaming && renamingPath != entry.path().string())
                {
                    RenameSelectedFile(renamingPath, newName);
                    renaming = false;
                }

                SelectionManager::SetSelectedFile(entry.path().string());
                renaming = false;
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                currentDirectory = entry.path().string();
            }
        }
        else
        {
            if (entry.path().extension() == ".fbx" || entry.path().extension() == ".png"
                || entry.path().extension() == ".mat")
            {
                if (ImGui::Selectable(entry.path().filename().string().c_str(), isSelected))
                {
                    if (renaming && renamingPath != entry.path().string())
                    {
                        RenameSelectedFile(renamingPath, newName);
                        renaming = false;
                    }

                    SelectionManager::SetSelectedFile(entry.path().string());
                    renaming = false;
                }

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    if (entry.path().extension() == ".png")
                    {
                        std::string command = "rundll32.exe \"%ProgramFiles%\\Windows Photo Viewer\\PhotoViewer.dll\", ImageView_Fullscreen " + entry.path().string();
                        system(command.c_str());
                    }
                    else
                    {
                        currentDirectory = entry.path().parent_path().string();
                    }
                }

                if (entry.path().extension() == ".fbx")
                {
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                    {
                        const std::string filePath = entry.path().string();
                        ImGui::SetDragDropPayload("FBX_FILE", filePath.c_str(), filePath.size() + 1);
                        ImGui::Text("Dragging %s", entry.path().filename().string().c_str());
                        ImGui::EndDragDropSource();
                    }
                }

                if (entry.path().extension() == ".mat")
                {

                }
            }
            else
            {
                if (ImGui::Selectable(entry.path().filename().string().c_str(), isSelected))
                {
                    if (renaming && renamingPath != entry.path().string())
                    {
                        RenameSelectedFile(renamingPath, newName);
                        renaming = false;
                    }

                    SelectionManager::SetSelectedFile(entry.path().string());
                    renaming = false;
                }
            }
        }

        if (isSelected && !renaming)
        {
            if (ImGui::IsItemClicked())
            {
                renaming = true;
                renamingPath = entry.path().string();
                strncpy_s(newName, entry.path().filename().string().c_str(), sizeof(newName));
            }
        }

        if (renaming && renamingPath == entry.path().string())
        {
            if (ImGui::InputText("##rename", newName, sizeof(newName), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                RenameSelectedFile(entry.path(), newName);
                renaming = false;
            }
            if (ImGui::IsMouseClicked(0) && !ImGui::IsItemHovered())
            {
                RenameSelectedFile(entry.path(), newName);
                renaming = false;
            }
        }

        ImGui::PopID();
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