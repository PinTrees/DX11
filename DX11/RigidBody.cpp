#include "pch.h"
#include "RigidBody.h"
#include "Debug.h"

RigidBody::RigidBody()
	: m_Velocity(0.0f, 0.0f, 0.0f),
	m_Acceleration(0.0f, 0.0f, 0.0f),
	m_Mass(1.0f),
	m_Elasticity(0.5f),
	m_IsKinematic(false)
{
	m_InspectorTitleName = "RigidBody";
}

RigidBody::~RigidBody()
{
}

void RigidBody::ApplyForce(const Vec3& force)
{
	Vec3 acceleration = force / m_Mass; 
	m_Acceleration += acceleration; 
}

void RigidBody::ApplyImpulse(const Vec3& impulse)
{
	if (m_IsKinematic)
		return;

	m_Velocity += impulse / m_Mass;
}

void RigidBody::Integrate(float deltaTime)
{
	if (m_IsKinematic)
		return;

	// �ӵ� ������Ʈ: v = u + at
	m_Velocity += m_Acceleration * deltaTime; 

	//Debug::Log("Acceleration: " + ToString(m_Acceleration)); 
	//Debug::Log("m_Velocity: " + ToString(m_Velocity)); 
	//Debug::Log("deltaTile: " + to_string(deltaTime)); 

	// ��ġ ������Ʈ: s = s + vt
	GetGameObject()->GetTransform()->Translate(m_Velocity * deltaTime);

	// ���ӵ� �ʱ�ȭ (�� ������ ���ȸ� ����)
	m_Acceleration = Vec3::Zero;

	int a = 0;
} 

void RigidBody::OnDrawGizmos()
{
	Transform* tr = GetGameObject()->GetTransform();
	Gizmo::DrawVector(tr->GetWorldMatrix(), m_Velocity);
}

void RigidBody::OnInspectorGUI()
{
	ImGui::DragFloat("Mass", &m_Mass);
	ImGui::DragFloat("Elasticity", &m_Elasticity);
	ImGui::Checkbox("Is Kinematic", &m_IsKinematic); 
}

GENERATE_COMPONENT_FUNC_TOJSON(RigidBody)
{
	json j = {};
	j["type"] = "RigidBody";
	j["mass"] = m_Mass;
	j["elasticity"] = m_Elasticity;
	j["isKinematic"] = m_IsKinematic; // Ű��ƽ ���� ����

	// Dont save velocity
	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(RigidBody)
{
	m_Mass = j.value("mass", 1.0f);					// Ű�� ���� ��� �⺻�� 1.0f ��� 
	m_Elasticity = j.value("elasticity", 0.3f);		// Ű�� ���� ��� �⺻�� 0.3f ��� 
	m_IsKinematic = j.value("isKinematic", false);	// Ű�� ���� ��� �⺻�� false ��� 
}
