#include "pch.h"
#include "Component.h"
#include "EditorGUI.h"

int Component::nextInstanceId = 0; 

Component::Component() 
	: m_InstanceId(nextInstanceId++) 
{ 
}

Component::~Component()
{
}

void Component::RenderInspectorGUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	EditorGUI::ComponentDivider();
	bool isOpened = EditorGUI::ComponentHeader(m_InspectorTitleName);
	EditorGUI::Divider(Color(0.15f, 0.15f, 0.15f, 1.0f), 1.4f);

	if (isOpened)
	{
		ImGui::PushID(m_InstanceId + GetInstanceID());
		EditorGUI::ComponentBlockStylePush();
		EditorGUI::Spacing(Vec2(0, 4));

		OnInspectorGUI();

		EditorGUI::Spacing(Vec2(0, 4));
		EditorGUI::ComponentBlockStylePop();
		ImGui::PopID();
	}

	ImGui::PopStyleVar();
}
