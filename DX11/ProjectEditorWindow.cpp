#include "pch.h"
#include "ProjectEditorWindow.h"
#include "Utils.h"
#include <functional> // std::hash
#include "EditorGUI.h"

bool isDragging = false;

wchar_t  ProjectEditorWindow::newName[512] = L"";
bool ProjectEditorWindow::renaming = false;
std::wstring ProjectEditorWindow::renamingPath = L"";

std::wstring ProjectEditorWindow::solutionDirectory = L"";
std::wstring ProjectEditorWindow::currentDirectory = L".";

vector<char> ProjectEditorWindow::renameBuffVec = {};

// Custom splitter function
bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2)
{
    ImVec2 backup_pos = ImGui::GetCursorPos();
    if (split_vertically)
        ImGui::SetCursorPosY(backup_pos.y + *size1);
    else
        ImGui::SetCursorPosX(backup_pos.x + *size1);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Separator]);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_SeparatorHovered]);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyle().Colors[ImGuiCol_SeparatorActive]);
    bool item_active = ImGui::Button("##Splitter", ImVec2(split_vertically ? -1 : thickness, split_vertically ? thickness : -1));
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    ImGui::SetItemAllowOverlap();
    if (item_active)
    {
        ImGui::SetMouseCursor(split_vertically ? ImGuiMouseCursor_ResizeNS : ImGuiMouseCursor_ResizeEW);
    }

    bool item_hovered = ImGui::IsItemHovered();
    if (item_hovered || item_active)
    {
        ImGui::SetMouseCursor(split_vertically ? ImGuiMouseCursor_ResizeNS : ImGuiMouseCursor_ResizeEW);
    }

    ImGui::SetCursorPos(backup_pos);
    if (item_active)
    {
        float mouse_delta = split_vertically ? ImGui::GetIO().MouseDelta.y : ImGui::GetIO().MouseDelta.x;
        if (mouse_delta != 0.0f)
        {
            if (mouse_delta < min_size1 - *size1)
                mouse_delta = min_size1 - *size1;
            if (mouse_delta > *size2 - min_size2)
                mouse_delta = *size2 - min_size2;
            *size1 += mouse_delta;
            *size2 -= mouse_delta;
            return true;
        }
    }
    return false;
}

ProjectEditorWindow::ProjectEditorWindow()
    : EditorWindow("Project", ICON_FA_FOLDER)
{
    //solutionDirectory = Application::GetDataPath();
    solutionDirectory = PathManager::GetI()->GetMovePathW(L"Assets\\");
    currentDirectory = solutionDirectory;
}

ProjectEditorWindow::~ProjectEditorWindow()
{
}

void ProjectEditorWindow::OnRender()
{
    // ���� ��θ� ��ܿ� ǥ��
    string aa = wstring_to_string(currentDirectory);
    ImGui::Text("Current Directory: %s", aa.c_str());

    static float leftPaneWidth = 250.0f; // ���� �г��� �ʱ� �ʺ�
    float minPaneWidth = 100.0f;         // �ּ� �г� �ʺ�
    float maxPaneWidth = ImGui::GetWindowWidth() - minPaneWidth; // �ִ� �г� �ʺ�

    // ���ø����� �β��� �巡�� ���� ����
    float splitterWidth = 4.0f;
    static bool isSplitterActive = false;

    // ���� ���� Ʈ�� ��
    ImGui::BeginChild("FolderTree", ImVec2(leftPaneWidth, 0), true);
    DisplayDirectoryTree(solutionDirectory);
    ImGui::EndChild();

    // ���ø��� ó��
    ImGui::SameLine();
    ImGui::InvisibleButton("Splitter", ImVec2(splitterWidth, -1));
    if (ImGui::IsItemActive())
    {
        isSplitterActive = true;
    }
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        isSplitterActive = false;
    }

    if (isSplitterActive)
    {
        float mouseDelta = ImGui::GetIO().MouseDelta.x;
        leftPaneWidth += mouseDelta;
        if (leftPaneWidth < minPaneWidth) leftPaneWidth = minPaneWidth;
        if (leftPaneWidth > maxPaneWidth) leftPaneWidth = maxPaneWidth;
    }

    // �е� ����
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); // ������ ���� �е��� ����
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0)); // ������ ���� �е��� ����

    // ���ø��� �ð��� ǥ�� (���� ����)
    ImGui::SameLine(0, 0);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 splitterMin = ImGui::GetItemRectMin();
    ImVec2 splitterMax = ImGui::GetItemRectMax();
    drawList->AddRectFilled(splitterMin, splitterMax, ImGui::GetColorU32(ImGuiCol_Separator));

    // �е� ����
    ImGui::PopStyleVar(2);

    ImGui::SameLine();
    DisplayDirectoryContents(currentDirectory);

    if (ImGui::IsItemClicked(1)) // Right-click
    {
        ImGui::OpenPopup("ItemContextMenu");
    }

    // ���콺 ��ư�� �������� �� �巡�� ���� �ʱ�ȭ
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        isDragging = false;
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
                if (ImGui::IsItemHovered() && ImGui::IsItemClicked()) // ��� ����
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
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f)); // ��ο� ȸ��

    ImGui::BeginChild("DirectoryContents", ImVec2(0, 0), true);
    for (const auto& entry : fs::directory_iterator(directory))
    {
        bool isSelected = (SelectionManager::GetSelectedFile() == entry.path().wstring());

        // Set UID
        ImGui::PushID(entry.path().wstring().c_str());

        ImGui::Dummy(ImVec2(8, 0));
        ImGui::SameLine();
        RenderFileEntry(entry, isSelected);

        ImGui::PopID();
    }
    ImGui::EndChild();

    ImGui::PopStyleColor();
}

void ProjectEditorWindow::RenderFileEntry(const fs::directory_entry& entry, bool isSelected)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    string filename = wstring_to_string(entry.path().filename().wstring());
    string extention = entry.path().extension().string();

    if (entry.is_directory())
    {
        EditorGUI::Image(L"\\ProjectSetting\\icons\\icon_folder.png", ImVec2(18, 18));
        EditorGUI::RowSizedBox(8);
        RenderFileEntry_Directory(entry, isSelected);
    }
    // .mesh file owner
    else if (extention == ".fbx" || extention == ".FBX")
    {
        EditorGUI::Image(L"\\ProjectSetting\\icons\\icon_mesh.png", ImVec2(24, 24));
        EditorGUI::RowSizedBox(8);
        RenderFileEntry_FBX(entry, isSelected);
    }
    // .mesh file - Hidden
    else if (extention == ".mesh")
    {
        //EditorGUI::Image(L"\\ProjectSetting\\icons\\icon_json.png", ImVec2(18, 18));
        //EditorGUI::RowSizedBox(8);
        //if (ImGui::Selectable(filename.c_str(), isSelected))
        //{
        //}
    }
    else if (extention == ".png" || extention == ".PNG")
    {
        EditorGUI::Image(L"\\ProjectSetting\\icons\\icon_img.png", ImVec2(18, 18));
        EditorGUI::RowSizedBox(8);
        RenderFileEntry_PNG(entry, isSelected);
    }
    else if (extention == ".mat" || extention == ".MAT")
    {
        EditorGUI::Image(L"\\ProjectSetting\\icons\\icon_material.png", ImVec2(24, 24));
        EditorGUI::RowSizedBox(8);
        RenderFileEntry_MAT(entry, isSelected);
    }
    else if (extention == ".txt" || extention == ".TXT")
    {
        EditorGUI::Image(L"\\ProjectSetting\\icons\\icon_text.png", ImVec2(18, 18));
        EditorGUI::RowSizedBox(8);
        if (ImGui::Selectable(filename.c_str(), isSelected))
        {
        }
    }
    else if (extention == ".json" || extention == ".JSON")
    {
        EditorGUI::Image(L"\\ProjectSetting\\icons\\icon_json.png", ImVec2(18, 18));
        EditorGUI::RowSizedBox(8);
        if (ImGui::Selectable(filename.c_str(), isSelected))
        {
        }
    }
    else
    {
        EditorGUI::Image(L"\\ProjectSetting\\icons\\icon_text.png", ImVec2(18, 18));
        EditorGUI::RowSizedBox(8);
        if (ImGui::Selectable(filename.c_str(), isSelected))
        {
        }
    }

    ImGui::PopStyleVar();
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






void ProjectEditorWindow::RenderFileEntry_Directory(const fs::directory_entry& entry, bool isSelected)
{
    wstring filename = entry.path().filename().wstring();

    ImGui::Selectable(wstring_to_string(filename).c_str(), isSelected);

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        currentDirectory = entry.path().wstring();
    }

    // �巡�� �� ��� Ÿ�� ����
    if (!isSelected && ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MAT_FILE"))
        {
            std::string receivedFilePath(static_cast<const char*>(payload->Data), payload->DataSize);
            std::wstring receivedFilePath_w = string_to_wstring(receivedFilePath);
        }
        else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PNG_FILE"))
        {
        }
        ImGui::EndDragDropTarget();
    }
}

void ProjectEditorWindow::RenderFileEntry_FBX(const fs::directory_entry& entry, bool isSelected)
{
    wstring filename = entry.path().filename().wstring();

    wstring path = PathManager::GetI()->GetCutSolutionPath(entry.path().wstring());

    shared_ptr<MeshFile> mesh_fbx = ResourceManager::GetI()->LoadFbxModel(wstring_to_string(path));

    if (mesh_fbx == nullptr)
        return;

    auto treeFlag = ImGuiTreeNodeFlags_OpenOnArrow
        | ImGuiTreeNodeFlags_SpanAvailWidth
        | (isSelected ? ImGuiTreeNodeFlags_DefaultOpen : 0)
        | (isSelected ? ImGuiTreeNodeFlags_Selected : 0);

    // TreeNode�� ���¸� ����
    bool treeNodeOpened = ImGui::TreeNodeEx(wstring_to_string(filename).c_str(), treeFlag);

    if (!isSelected && ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
    {
        // ���� ���� ó��
        SelectionManager::SetSelectedFile(entry.path().wstring());
        isSelected = true;
    }

    // Ʈ�� ��忡 ���� �巡�� �� ��� �ҽ� ����
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
        const std::wstring filePath = entry.path().wstring();
        ImGui::SetDragDropPayload("FBX_FILE", filePath.c_str(), filePath.size() + 1);
        ImGui::Text("Dragging %s", filename.c_str());
        ImGui::EndDragDropSource();
    }

    if (treeNodeOpened)
    {
        for (size_t i = 0; i < mesh_fbx->SkinnedMeshs.size(); ++i)
        {
            const auto& subset = mesh_fbx->SkinnedMeshs[i]; 
            std::wstring subsetName = L"Skinned - " + string_to_wstring(subset->Name);

            bool isSubsetSelected = (SelectionManager::GetSelectedFile() == (filename + L"\\" + subsetName));
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::Selectable(wstring_to_string(subsetName).c_str(), isSubsetSelected))
            {
                SelectionManager::SetSelectedFile(filename + L"\\" + subsetName);
            }

            // ����¿� ���� �巡�� �� ��� �ҽ� ����
            if (ImGui::IsItemActive() && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
            {
                const std::string subsetPath = entry.path().string() + "\\" + std::to_string(i);
                ImGui::SetDragDropPayload("FBX_SKINNED_MESH", subsetPath.c_str(), subsetPath.size() + 1);
                ImGui::Text("Dragging %s", wstring_to_string(subsetName).c_str());
                ImGui::EndDragDropSource();
            }

            ImGui::PopID();
        }
        for (size_t i = 0; i < mesh_fbx->Meshs.size(); ++i)
        {
            const auto& subset = mesh_fbx->Meshs[i];
            std::wstring subsetName = L"Static - " + string_to_wstring(subset->Name);

            bool isSubsetSelected = (SelectionManager::GetSelectedFile() == (filename + L"\\" + subsetName));
            ImGui::PushID(static_cast<int>(i));
            if (ImGui::Selectable(wstring_to_string(subsetName).c_str(), isSubsetSelected))
            {
                SelectionManager::SetSelectedFile(filename + L"\\" + subsetName);
            }

            // ����¿� ���� �巡�� �� ��� �ҽ� ����
            if (ImGui::IsItemActive() && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
            {
                const std::string subsetPath = entry.path().string() + "\\" + std::to_string(i);
                ImGui::SetDragDropPayload("FBX_MESH", subsetPath.c_str(), subsetPath.size() + 1);
                ImGui::Text("Dragging %s", wstring_to_string(subsetName).c_str());
                ImGui::EndDragDropSource();
            }
            ImGui::PopID();
        }
        for (size_t i = 0; i < mesh_fbx->SkinnedData.AnimationClips.size(); ++i) 
        {
            auto animationClip = mesh_fbx->SkinnedData.AnimationClips[i]; 
            std::wstring subsetName = L"AnimationClip - " + string_to_wstring(animationClip.Name); 
        
            bool isSubsetSelected = (SelectionManager::GetSelectedFile() == (filename + L"\\" + subsetName));  
            ImGui::PushID(static_cast<int>(i)); 
            if (ImGui::Selectable(wstring_to_string(subsetName).c_str(), isSubsetSelected)) 
            {
                SelectionManager::SetSelectedFile(filename + L"\\" + subsetName);
            } 
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}

void ProjectEditorWindow::RenderFileEntry_PNG(const fs::directory_entry& entry, bool isSelected)
{
    wstring filename = entry.path().filename().wstring();

    ImGui::Selectable(wstring_to_string(filename).c_str(), isSelected);

    if (!isSelected && ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
    {
        // ���� ���� ó��
        SelectionManager::SetSelectedFile(entry.path().wstring());
        isSelected = true;
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        // ���� Ŭ�� ó��

        std::wstring command = L"ms-photos:viewer?filePath=" + entry.path().wstring();
        ShellExecute(0, 0, command.c_str(), 0, 0, SW_SHOWNORMAL);
    }
    else if (isSelected && ImGui::BeginDragDropSource())
    {
        // �巡�� �� ��� ó��

        const std::string filePath = wstring_to_string(entry.path().wstring());
        const std::string filename_d = wstring_to_string(entry.path().filename().wstring());

        // ���� ��θ� ���̷ε�� ����
        ImGui::SetDragDropPayload("PNG_FILE", filePath.c_str(), (filePath.size() + 1) * sizeof(char));
        ImGui::Text("Dragging %s", filename_d.c_str());
        ImGui::EndDragDropSource();
    }
}

void ProjectEditorWindow::RenderFileEntry_MAT(const fs::directory_entry& entry, bool isSelected)
{
    wstring filename = entry.path().filename().wstring();

    ImGui::Selectable(wstring_to_string(filename).c_str(), isSelected);

    if (!isSelected && ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
    {
        // ���� ���� ó��
        SelectionManager::SetSelectedFile(entry.path().wstring());
        isSelected = true;
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        // ���� Ŭ�� ó��
    }

    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("MAT_FILE", NULL, 0);
        ImGui::Text("This is a drag and drop source");
        ImGui::EndDragDropSource();
    }

    //if (isSelected && ImGui::BeginDragDropSource())
    //{
    //    const std::string filePath = wstring_to_string(entry.path().wstring());
    //    const std::string filename_d = wstring_to_string(entry.path().filename().wstring());
    //
    //    if (!isDragging)
    //    {
    //        isDragging = true;
    //        ImGui::SetDragDropPayload("FILE", filePath.c_str(), (filePath.size() + 1) * sizeof(char));
    //    }
    //
    //    ImGui::Text("Dragging %s", filename_d.c_str());
    //    ImGui::EndDragDropSource();
    //}
}