#include "pch.h"
#include "Component.h"

Component::Component()
{
}

Component::~Component()
{
}

void Component::RenderInspectorGUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f); 
	bool isOpened = ImGui::CollapsingHeader(m_InspectorTitleName.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

	if (isOpened)
	{
		OnInspectorGUI();
	}
	ImGui::PopStyleVar();
}
