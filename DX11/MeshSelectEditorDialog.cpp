#include "pch.h"
#include "MeshSelectEditorDialog.h"
#include "EditorGUIManager.h"
#include "EditorGUI.h"

MeshSelectEditorDialog::MeshSelectEditorDialog(
    Mesh* selectMesh, 
    SkinnedMesh* selectSkinnedMesh, 
    int& selectIndex) : EditorDialog("Mesh Select")
    , m_MeshFiles{}
    , m_SelectMesh(selectMesh)
    , m_SelectSkinnedMesh(selectSkinnedMesh)
    , m_SelectSubsetIndex(selectIndex)
{
}

MeshSelectEditorDialog::~MeshSelectEditorDialog()
{
}

void MeshSelectEditorDialog::Open(
    Mesh*        selectMesh,
    SkinnedMesh* selectSkinnedMesh,
    int&         selectSubsetIndex)
{
	MeshSelectEditorDialog* view = new MeshSelectEditorDialog(selectMesh, selectSkinnedMesh, selectSubsetIndex);
	view->Show();   
}

void MeshSelectEditorDialog::Show()
{
    EditorDialog::Show();

    std::string path = wstring_to_string(PathManager::GetI()->GetContentPathW()) + "Assets\\";   // Ž���� ���  
    std::string extension = ".mesh";                                                            // Ž���� ���� Ȯ����

    // ���͸� Ž��
    for (const auto& entry : fs::recursive_directory_iterator(path))
    {
        // ������ .mesh Ȯ���ڸ� �������� Ȯ��
        if (entry.is_regular_file() && entry.path().extension() == extension)
        {
            std::string filePath = entry.path().string(); // ���� ���
            string assetPath = PathManager::GetI()->GetCutSolutionPath(filePath); 

            // .mesh ���� �ε�
            auto meshFile = ResourceManager::GetI()->LoadMeshFile(assetPath); 

            // �ε��� ������ m_MeshFiles ����Ʈ�� �߰�
            m_MeshFiles.push_back(meshFile);
        }
    }
}

void MeshSelectEditorDialog::Close()
{
    EditorDialog::Close(); 
}

void MeshSelectEditorDialog::OnRender()
{
    static Mesh* selectMesh = nullptr;
    static SkinnedMesh* selectSkinnedMesh = nullptr;

    // ��ũ�� ������ ������ ���� (ID�� �ο�)
    ImGui::BeginChild("ScrollableRegion", ImVec2(0, -40), true, ImGuiWindowFlags_HorizontalScrollbar);

    for (auto meshFile : m_MeshFiles)
    {
        //EditorGUI::Label(meshFile->Name);
        //EditorGUI::Label(wstring_to_string(meshFile->Path)); 
        //EditorGUI::Label(meshFile->FullPath);
        //EditorGUI::Label(to_string(meshFile->Meshs.size()));
        //EditorGUI::Label(to_string(meshFile->SkinnedMeshs.size())); 

        if (m_SelectMesh)
        {
            for (auto mesh : meshFile->Meshs)
            {
                EditorGUI::Image(L"\\ProjectSetting\\icons\\icon_mesh.png", ImVec2(18, 18));
                EditorGUI::RowSizedBox(8);
                // ���� ������ ��� ����
                if (ImGui::Selectable(("Static - " + mesh->Name).c_str(), selectMesh == mesh))
                {
                    selectMesh = mesh;
                }
            }
        }
        if (m_SelectSkinnedMesh)
        {
            for (auto skinnedMesh : meshFile->SkinnedMeshs)
            {
                EditorGUI::Image(L"\\ProjectSetting\\icons\\icon_mesh.png", ImVec2(18, 18));
                EditorGUI::RowSizedBox(8);
                // ���� ������ ��� ����
                if (ImGui::Selectable(("Skinned - " + skinnedMesh->Name).c_str(), selectSkinnedMesh == skinnedMesh))
                {
                    selectSkinnedMesh = skinnedMesh;
                }
            }
        }
    }
    ImGui::EndChild(); // ��ũ�� ������ ���� ��

    if (EditorGUI::Button("Select")) 
    {
        if (m_SelectSkinnedMesh) 
        { 
            m_SelectSkinnedMesh = selectSkinnedMesh;  
        } 
        Close();  
    }
}
