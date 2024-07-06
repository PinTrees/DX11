#include "pch.h"
#include "ProjectEditorWindow.h"
#include "Utils.h"

wchar_t  ProjectEditorWindow::newName[512] = L"";
bool ProjectEditorWindow::renaming = false;
std::wstring ProjectEditorWindow::renamingPath = L"";

std::wstring ProjectEditorWindow::solutionDirectory = L""; 
std::wstring ProjectEditorWindow::currentDirectory = L"."; 

vector<char> ProjectEditorWindow::renameBuffVec = {};

std::string WStringToString(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}


ProjectEditorWindow::ProjectEditorWindow()
    : EditorWindow("Project")
{
    solutionDirectory = Application::GetDataPath();
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
                UMaterial::Create(wstring_to_string(currentDirectory));
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Folder"))
            {
                std::wstring folderPath = currentDirectory + L"/New Folder";
                int folderIndex = 1;
                while (std::filesystem::exists(folderPath)) 
                {
                    folderPath = currentDirectory + L"/New Folder (" + std::to_wstring(folderIndex++) + L")";
                }
                std::filesystem::create_directory(folderPath);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
}

void ProjectEditorWindow::DisplayDirectoryTree(const std::wstring& directory)
{
    for (const auto& entry : fs::directory_iterator(directory))
    {
        std::string filename = wstring_to_string(entry.path().filename().wstring());

        if (entry.is_directory())
        {
            if (ImGui::TreeNode(filename.c_str()))
            {
                if (ImGui::IsItemClicked())
                {
                    currentDirectory = entry.path().wstring();
                }

                DisplayDirectoryTree(entry.path().wstring());
                ImGui::TreePop();
            }
        }
    }
}

void ProjectEditorWindow::DisplayDirectoryContents(const std::wstring& directory)
{
    for (const auto& entry : fs::directory_iterator(directory))
    {
        bool isSelected = (SelectionManager::GetSelectedFile() == entry.path().wstring());

        ImGui::PushID(entry.path().wstring().c_str()); 

        RenderFileEntry(entry, isSelected);

        ImGui::PopID();
    }
}

void ProjectEditorWindow::RenderFileEntry(const fs::directory_entry& entry, bool isSelected)
{
    std::string filename = WStringToString(entry.path().filename().wstring());

    if (renaming && renamingPath == entry.path().wstring() && isSelected)
    {
        // �̸� ���� UI ǥ��
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        //bool renameInputActive = ImGui::IsItemActive();
        //
        //// Ŀ���� �ִ��� Ȯ���Ͽ� �ʿ� �� ��Ŀ�� ����
        //if (!renameInputActive && !ImGui::IsAnyItemActive()) {
        //    ImGui::SetKeyboardFocusHere();
        //}

        if (ImGui::InputText("##rename", renameBuffVec.data(), renameBuffVec.size(), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            std::wstring newNameWstring = string_to_wstring(std::string(renameBuffVec.data()));

            fs::path renamingPathFs(renamingPath);
            fs::rename(renamingPathFs, renamingPathFs.parent_path() / newNameWstring);
            
            renaming = false;
        }

        if (ImGui::IsMouseClicked(0) && !ImGui::IsItemHovered())
        {
            std::wstring newNameWstring = string_to_wstring(std::string(renameBuffVec.data())); 

            fs::path renamingPathFs(renamingPath); 
            fs::rename(renamingPathFs, renamingPathFs.parent_path() / newNameWstring); 
            
            renaming = false;
        }

        ImGui::PopStyleVar();

        return;
    }
    else
    {
        if (ImGui::Selectable(filename.c_str(), isSelected))
        {
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
        {
            SelectionManager::SetSelectedFile(entry.path().wstring());
            
            if (renaming)
            {
                std::wstring newNameWstring = string_to_wstring(std::string(renameBuffVec.data()));

                fs::path renamingPathFs(renamingPath);
                fs::rename(renamingPathFs, renamingPathFs.parent_path() / newNameWstring);
                renaming = false;
            }
        }

        if (entry.is_directory())
        {
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                currentDirectory = entry.path().wstring();
            }
        }
        else
        {
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                if (entry.path().extension() == ".png")
                {
                    std::string command = "rundll32.exe \"%ProgramFiles%\\Windows Photo Viewer\\PhotoViewer.dll\", ImageView_Fullscreen " + entry.path().string();
                    system(command.c_str());
                }
                else
                {
                    currentDirectory = entry.path().parent_path().wstring();
                }
            }

            if (entry.path().extension() == ".fbx")
            {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    const std::string filePath = entry.path().string();
                    ImGui::SetDragDropPayload("FBX_FILE", filePath.c_str(), filePath.size() + 1);
                    ImGui::Text("Dragging %s", filename.c_str());
                    ImGui::EndDragDropSource();
                }
            }

            if (entry.path().extension() == ".mat")
            {
                // Handle .mat files if necessary
            }
        }

        if (isSelected && !renaming)
        {
            if (ImGui::IsItemClicked() && !ImGui::IsMouseDoubleClicked(0))
            {
                renaming = true;
                renamingPath = entry.path().wstring();
               
                std::wstring wfilename = entry.path().filename().wstring();
                size_t len = wfilename.length();

                wcsncpy_s(newName, wfilename.c_str(), len);
                newName[len] = L'\0'; // Null-terminate the string

                std::string renameBuffer = WStringToString(newName);
                renameBuffVec.assign(renameBuffer.begin(), renameBuffer.end());
                renameBuffVec.resize(512, '\0'); // Resize to 512 with null terminator
            }
        }
    }
}

void ProjectEditorWindow::RenameSelectedFile(const std::wstring& oldPath, const std::wstring& newName)
{
    try
    {
        fs::path oldFilePath(oldPath);
        fs::path newFilePath = oldFilePath.parent_path() / newName;

        fs::rename(oldFilePath, newFilePath);
        SelectionManager::SetSelectedFile(newFilePath.wstring());
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "Error renaming file: " << e.what() << std::endl;
    }
}
