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

	// �� ���� �����Ͽ� �巡�� �� ��� ���� Ȯ��
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
	ImGui::PushID(gameObject->GetInstanceID()); // ���� ID�� ����Ͽ� ImGui ID ���ÿ� Ǫ��

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

	// �巡�� �� ��� ó�� (Ư�� ������Ʈ�� ���)
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FBX_FILE"))
		{
			const char* filePath = static_cast<const char*>(payload->Data);
			HandleFbxFileDrop(std::string(filePath), gameObject);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::PopID(); // ImGui ID ���ÿ��� ��
}

void SceneHierachyEditorWindow::HandleFbxFileDrop(const std::string& filePath, GameObject* parent)
{
	// ���� ��� ��ȿ�� �˻�
	std::filesystem::path path(filePath);
	if (!std::filesystem::exists(path))
	{
		std::cerr << "File does not exist: " << filePath << std::endl;
		return;
	}

	// ���� �̸� ���� (Ȯ���� ����)
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
