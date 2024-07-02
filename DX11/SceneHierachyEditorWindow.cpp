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

	// �� ���� �����Ͽ� �巡�� �� ��� ���� Ȯ��
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
	ImGui::PushID(gameObject->GetInstanceID()); // ���� ID�� ����Ͽ� ImGui ID ���ÿ� Ǫ��

	if (ImGui::TreeNode(gameObject->GetName().c_str()))
	{
		std::vector<GameObject*> children = gameObject->GetChildren();
		for (int i = 0; i < children.size(); ++i)
		{
			DrawGameObject(children[i]);
		}
		ImGui::TreePop();
	}

	// �巡�� �� ��� ó�� (Ư�� ������Ʈ�� ���)
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

	ImGui::PopID(); // ImGui ID ���ÿ��� ��
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
