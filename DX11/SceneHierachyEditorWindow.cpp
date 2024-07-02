#include "pch.h"
#include "SceneHierachyEditorWindow.h"


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
			IM_ASSERT(payload->DataSize == sizeof(std::string));
			std::string filePath = *(const std::string*)payload->Data;
			HandleFbxFileDrop(filePath, nullptr);
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

	if (ImGui::TreeNode(gameObject->GetName().c_str()))
	{
		std::vector<GameObject*> children = gameObject->GetChildren();
		for (int i = 0; i < children.size(); ++i)
		{
			DrawGameObject(children[i]);
		}
		ImGui::TreePop();
	}

	// 드래그 앤 드롭 처리 (특정 오브젝트에 드롭)
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FBX_FILE"))
		{
			IM_ASSERT(payload->DataSize == sizeof(std::string));
			std::string filePath = *(const std::string*)payload->Data;
			HandleFbxFileDrop(filePath, gameObject); 
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::PopID(); // ImGui ID 스택에서 팝
}

void SceneHierachyEditorWindow::HandleFbxFileDrop(const std::string& filePath, GameObject* parent)
{
	GameObject* newGameObject = new GameObject("AAAAA");

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
