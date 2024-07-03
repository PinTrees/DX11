#include "pch.h"
#include "InspectorEditorWindow.h"
#include <filesystem>

namespace fs = std::filesystem;

InspectorEditorWindow::InspectorEditorWindow()
	: EditorWindow("Inspector")
{
}

InspectorEditorWindow::~InspectorEditorWindow()
{
}

void InspectorEditorWindow::OnRender()
{
	if (SelectionManager::GetSelectedObjectType() == SelectionType::GAMEOBJECT)
	{
		GameObject* curSelectGameObject = SelectionManager::GetSelectedGameObject();

		if (curSelectGameObject == nullptr)
			return;

		const std::vector<shared_ptr<Component>>& components = curSelectGameObject->GetComponents();

		for (const auto& component : components)
		{
			component->OnInspectorGUI();
			ImGui::Separator();
		}
	}
	else if (SelectionManager::GetSelectedObjectType() == SelectionType::FILE)
	{
		if (SelectionManager::GetSelectedSubType() == SelectionSubType::MATERIAL)
		{
			UMaterial* material = SelectionManager::GetSelectMaterial();
			if (material == nullptr)
				return;

			material->OnInspectorGUI();
		}
	}
}
