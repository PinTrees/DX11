#include "pch.h"
#include "BoxCollider.h"

BoxCollider::BoxCollider()
    : m_Size(Vec3::One)
{

}

BoxCollider::~BoxCollider()
{

}

void BoxCollider::Render()
{
    Gizmo::DrawCube(m_pGameObject->GetComponent<Transform>()->GetWorldMatrix(), m_Size);
}

void BoxCollider::OnInspectorGUI()
{
    if (ImGui::CollapsingHeader("Box Collider"))
    {
        Vec3 size = m_Size;

        if (ImGui::DragFloat3("Size", &size.x))
        {
            SetSize(size);
        }
    }
}
