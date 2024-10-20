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

    std::string path = wstring_to_string(PathManager::GetI()->GetContentPathW()) + "Assets\\";   // 탐색할 경로  
    std::string extension = ".mesh";                                                            // 탐색할 파일 확장자

    // 디렉터리 탐색
    for (const auto& entry : fs::recursive_directory_iterator(path))
    {
        // 파일이 .mesh 확장자를 가졌는지 확인
        if (entry.is_regular_file() && entry.path().extension() == extension)
        {
            std::string filePath = entry.path().string(); // 파일 경로
            string assetPath = PathManager::GetI()->GetCutSolutionPath(filePath); 

            // .mesh 파일 로드
            auto meshFile = ResourceManager::GetI()->LoadMeshFile(assetPath); 

            // 로드한 파일을 m_MeshFiles 리스트에 추가
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

    // 스크롤 가능한 영역을 시작 (ID를 부여)
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
                // 선택 가능한 요소 생성
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
                // 선택 가능한 요소 생성
                if (ImGui::Selectable(("Skinned - " + skinnedMesh->Name).c_str(), selectSkinnedMesh == skinnedMesh))
                {
                    selectSkinnedMesh = skinnedMesh;
                }
            }
        }
    }
    ImGui::EndChild(); // 스크롤 가능한 영역 끝

    if (EditorGUI::Button("Select")) 
    {
        if (m_SelectSkinnedMesh) 
        { 
            m_SelectSkinnedMesh = selectSkinnedMesh;  
        } 
        Close();  
    }
}
