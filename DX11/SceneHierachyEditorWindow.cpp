#include "pch.h"
#include "SceneHierachyEditorWindow.h"
#include <filesystem>
#include "EditorGUI.h"

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

	// ��ü ȭ���� �巡�� �� ��� �������� Ȯ��
	ImVec2 windowPos = ImGui::GetWindowPos(); // â�� ��ġ
	ImVec2 windowSize = ImGui::GetWindowSize(); // â�� ũ��

	ImGui::SetCursorPos(ImVec2(0, 0)); // Ŀ���� â�� �»������ �̵�
	ImGui::InvisibleButton("##DragDropSpace", windowSize); // â ũ�⸸ŭ ������ �ʴ� ��ư ����

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FBX_FILE"))
		{
			const char* filePath = static_cast<const char*>(payload->Data);
			HandleFbxFileDrop(std::string(filePath), nullptr);
		}

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAME_OBJECT"))
		{
			GameObject* droppedObject = *(GameObject**)payload->Data;
			droppedObject->SetParent(nullptr);
		} 
		ImGui::EndDragDropTarget(); 
	}

	// ��Ŭ�� �޴� �߰�
	if (ImGui::BeginPopupContextWindow("##SceneHierarchyContextMenu"))
	{
		if (ImGui::MenuItem("Add Camera"))
		{
			// ī�޶� �߰� ����
			GameObject* newCamera = new GameObject("Camera");
			currentScene->AddRootGameObject(newCamera);
			//newCamera->AddComponent<CameraComponent>();
			//currentScene->AddGameObject(newCamera);
		}
		if (ImGui::MenuItem("Add Directinal Light"))
		{
			// ����Ʈ �߰� ����
			GameObject* newLight = new GameObject("Directinal Light");
			currentScene->AddRootGameObject(newLight);
		}
		if (ImGui::MenuItem("Add Empty GameObject"))
		{
			// �� ���� ������Ʈ �߰� ����
			GameObject* newEmpty = new GameObject("GameObject");
			currentScene->AddRootGameObject(newEmpty);
		}
		ImGui::EndPopup();
	}

	// ��Ŭ�� �����Ͽ� �˾� �޴� ����
	if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered())
	{
		ImGui::OpenPopup("##SceneHierarchyContextMenu");
	}
}


void SceneHierachyEditorWindow::DrawGameObject(GameObject* gameObject)
{
	ImGui::PushID(gameObject->GetInstanceID()); 
	
	ImGui::Dummy(ImVec2(8, 0));
	ImGui::SameLine();
	EditorGUI::Image(L"\\ProjectSetting\\icons\\icon_gameobject.png", ImVec2(18, 18));
	ImGui::SameLine();

	bool isSelected = (SelectionManager::GetSelectedGameObject() == gameObject);

	if (ImGui::TreeNodeEx(gameObject->GetName().c_str(), isSelected ? ImGuiTreeNodeFlags_Selected : 0))
	{
		if (ImGui::IsItemClicked())
		{
			SelectionManager::SetSelectedGameObject(gameObject);
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
		SelectionManager::SetSelectedGameObject(gameObject);
	}

	// �巡�� �� ��� �ҽ�
	if (ImGui::BeginDragDropSource()) 
	{
		ImGui::SetDragDropPayload("GAME_OBJECT", &gameObject, sizeof(GameObject*)); 
		ImGui::Text("Dragging %s", gameObject->GetName().c_str()); 
		ImGui::EndDragDropSource(); 
	}

	// �巡�� �� ��� ó�� (Ư�� ������Ʈ�� ���)
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FBX_FILE"))
		{
			const char* filePath = static_cast<const char*>(payload->Data);
			HandleFbxFileDrop(std::string(filePath), gameObject);
		}

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAME_OBJECT")) 
		{
			GameObject* droppedObject = *(GameObject**)payload->Data; 
			droppedObject->SetParent(gameObject); 
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
