#include "pch.h"
#include "SceneHierachyEditorWindow.h"
#include <filesystem>

SceneHierachyEditorWindow::SceneHierachyEditorWindow()
	: EditorWindow("SceneHerachy")
{
}

SceneHierachyEditorWindow::~SceneHierachyEditorWindow()
{
}

void SceneHierachyEditorWindow::OnRender()
{
	Scene* currentScene = SceneManager::GetI()->GetCurrentScene(); 

	if (currentScene == nullptr)
		return;

	vector<GameObject*> rootGameObjects = currentScene->GetRootGameObjects();

	for (int i = 0; i < rootGameObjects.size(); ++i)
	{
		GameObject* gameObject = rootGameObjects[i];
		DrawGameObject(gameObject);
	}

	// 빈 공간 생성하여 드래그 앤 드롭 영역 확장
	ImVec2 availableSpace = ImGui::GetContentRegionAvail();
	ImGui::InvisibleButton("##DragDropSpace", availableSpace);

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FBX_FILE"))
		{
			const char* filePath = static_cast<const char*>(payload->Data);
			HandleFbxFileDrop(std::string(filePath), nullptr);
		}
		ImGui::EndDragDropTarget();
	}
}

void SceneHierachyEditorWindow::OnRenderExit()
{
}


void SceneHierachyEditorWindow::DrawGameObject(GameObject* gameObject)
{
	ImGui::PushID(gameObject->GetInstanceID()); // 고유 ID를 사용하여 ImGui ID 스택에 푸시

	bool isSelected = (Application::GetI()->GetCurSelectGameObject() == gameObject);

	if (ImGui::TreeNodeEx(gameObject->GetName().c_str(), isSelected ? ImGuiTreeNodeFlags_Selected : 0))
	{
		if (ImGui::IsItemClicked())
		{
			Application::GetI()->SetCurSelectGameObject(gameObject);
		}

		std::vector<GameObject*> children = gameObject->GetChildren();
		for (int i = 0; i < children.size(); ++i)
		{
			DrawGameObject(children[i]);
		}
		ImGui::TreePop();
	}
	else if (ImGui::IsItemClicked())
	{
		Application::GetI()->SetCurSelectGameObject(gameObject);
	}

	// 드래그 앤 드롭 처리 (특정 오브젝트에 드롭)
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FBX_FILE"))
		{
			const char* filePath = static_cast<const char*>(payload->Data);
			HandleFbxFileDrop(std::string(filePath), gameObject);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::PopID(); // ImGui ID 스택에서 팝
}

void SceneHierachyEditorWindow::HandleFbxFileDrop(const std::string& filePath, GameObject* parent)
{
	// 파일 경로 유효성 검사
	std::filesystem::path path(filePath);
	if (!std::filesystem::exists(path))
	{
		std::cerr << "File does not exist: " << filePath << std::endl;
		return;
	}

	// 파일 이름 추출 (확장자 제외)
	std::string fileName = path.stem().string();

	GameObject* newGameObject = new GameObject(fileName);

	Scene* currentScene = SceneManager::GetI()->GetCurrentScene();
	if (currentScene != nullptr)
	{
		if (parent == nullptr)
		{
			currentScene->AddRootGameObject(newGameObject);
		}
		else
		{
			newGameObject->SetParent(parent);
		}
	}
}
