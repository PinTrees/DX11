#include "pch.h"
#include "Component.h"
#include "EditorGUI.h"

int Component::nextInstanceId = 0; 

Component::Component()  
	: m_InstanceId(nextInstanceId++) 
	, m_pGameObject(nullptr)
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

	// �巡�� �� ��� �ҽ� (�巡���� ������Ʈ)
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		// ������Ʈ�� �ּҸ� ���̷ε忡 ����
		Component* component = this; 
		ImGui::SetDragDropPayload("COMPONENT_DRAG", &component, sizeof(Component*)); 
		ImGui::Text("Dragging: %s", m_InspectorTitleName.c_str());  
		ImGui::EndDragDropSource();
	}

	EditorGUI::Divider(Color(0.15f, 0.15f, 0.15f, 1.0f), 1.4f);

	EditorGUI::Label("Instance ID: " + to_string(m_InstanceId)); 

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
