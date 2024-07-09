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

	// 초기화 시 회전관성 텐서와 그 역행렬을 설정
	m_InertiaTensor = Matrix::Identity;
	m_InverseInertiaTensor = Matrix::Identity;
	m_CenterOfMass = Vec3::Zero;
}

RigidBody::~RigidBody()
{
}

void RigidBody::ApplyTorque(const Vec3& torque)
{
	if (m_IsKinematic) return; // 키네틱이면 토크를 적용하지 않음

	// 각가속도 업데이트
	Vec3 angularAcceleration = Vec3::Transform(torque, m_InverseInertiaTensor); 
	m_AngularAcceleration += angularAcceleration;
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

	if (m_IsKinematic || m_Sleep) return; // 키네틱이면 물리 계산을 하지 않음

	// 공기 저항 적용
	Vec3 airResistanceForce = -m_AirResistance * m_Velocity;
	ApplyForce(airResistanceForce);

	// 선속도 업데이트: v = u + at
	m_Velocity += m_Acceleration * deltaTime;

	// 각속도 업데이트: w = w + αt
	m_AngularVelocity += m_AngularAcceleration * deltaTime;

	// 위치 업데이트: s = s + vt
	GetGameObject()->GetTransform()->Translate(m_Velocity * deltaTime);

	// 회전 업데이트
	Quaternion rotation = GetGameObject()->GetTransform()->GetRotationQ();  
	Quaternion deltaRotation = Quaternion::CreateFromYawPitchRoll(m_AngularVelocity.y * deltaTime, m_AngularVelocity.x * deltaTime, m_AngularVelocity.z * deltaTime); 
	rotation = rotation * deltaRotation; 
	rotation.Normalize();
	GetGameObject()->GetTransform()->SetRotationQ(rotation);

	// 가속도 초기화 (한 프레임 동안만 적용)
	m_Acceleration = Vec3::Zero;
	m_AngularAcceleration = Vec3::Zero;
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
	ImGui::DragFloat("Air Resistance", &m_AirResistance);
	ImGui::DragFloat("Friction", &m_Friction);
	ImGui::Checkbox("Is Kinematic", &m_IsKinematic);
	ImGui::Checkbox("Is Movable", &m_IsMovable);
	ImGui::Checkbox("Sleep", &m_Sleep);
}

GENERATE_COMPONENT_FUNC_TOJSON(RigidBody)
{
	json j = {};
	j["type"] = "RigidBody";
	j["mass"] = m_Mass;
	j["elasticity"] = m_Elasticity;
	j["airResistance"] = m_AirResistance;
	j["friction"] = m_Friction;
	j["isKinematic"] = m_IsKinematic;
	j["isMovable"] = m_IsMovable;
	j["sleep"] = m_Sleep;

	// Dont save velocity
	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(RigidBody)
{
	m_Mass = j.value("mass", 1.0f);
	m_Elasticity = j.value("elasticity", 0.3f);
	m_AirResistance = j.value("airResistance", 0.1f);
	m_Friction = j.value("friction", 0.5f);
	m_IsKinematic = j.value("isKinematic", false);
	m_IsMovable = j.value("isMovable", true);
	m_Sleep = j.value("sleep", false);
}
