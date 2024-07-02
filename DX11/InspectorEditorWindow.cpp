#include "pch.h"
#include "InspectorEditorWindow.h"

InspectorEditorWindow::InspectorEditorWindow()
	: EditorWindow("Inspector")
{
}

InspectorEditorWindow::~InspectorEditorWindow()
{
}

void InspectorEditorWindow::OnRender()
{
	GameObject* curSelectGameObject = Application::GetI()->GetCurSelectGameObject();

	if (curSelectGameObject == nullptr)
		return;

	const std::vector<shared_ptr<Component>>& components = curSelectGameObject->GetComponents();

	for (const auto& component : components)
	{
		component->OnInspectorGUI();
		ImGui::Separator();
	}
}
