#include "pch.h"
#include "RigidBody.h"
#include "Debug.h"

RigidBody::RigidBody()
	: m_Velocity(0.0f, 0.0f, 0.0f),
	m_Acceleration(0.0f, 0.0f, 0.0f),
	m_Mass(1.0f),
	m_IsKinematic(false)
{
	m_InspectorTitleName = "RigidBody";

	// �ʱ�ȭ �� ȸ������ �ټ��� �� ������� ����
	m_InertiaTensor = Matrix::Identity;
	m_InverseInertiaTensor = Matrix::Identity;
	m_CenterOfMass = Vec3::Zero;
}

RigidBody::~RigidBody()
{
}

Vec3 RigidBody::GetCenterOfMass()
{
	return GetGameObject()->GetTransform()->GetPosition();
}

void RigidBody::ApplyTorque(const Vec3& torque)
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

Vec3 RigidBody::GetRotation()
{
	return GetGameObject()->GetTransform()->GetRotation();
}

void RigidBody::SetRotation(const Vec3& rotation)
{
	GetGameObject()->GetTransform()->SetRotation(rotation);
}

void RigidBody::TransformInertiaTensor()
{
	Matrix worldMatrix = GetGameObject()->GetTransform()->GetWorldMatrix();

	// Extract the rotation matrix (3x3) from the world matrix (4x4)
	Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(
		GetGameObject()->GetTransform()->GetRotation().y,
		GetGameObject()->GetTransform()->GetRotation().x,
		GetGameObject()->GetTransform()->GetRotation().z
	);

	// Transform inertia tensor from local space to world space
	m_InverseInertiaTensorWorld = rotationMatrix * m_InverseInertiaTensor * rotationMatrix.Transpose(); 
}

void RigidBody::Integrate(float deltaTime)
{
	if (m_IsKinematic) 
		return;

	/* ��ü�� ������ ���Ѵ��� ������ ���� �ʴ´� */
	if (GetInverseMass() == 0.0f) 
		return;

	/* ���ӵ��� ����Ѵ� */
	m_PrevAcceleration = m_Acceleration;
	m_PrevAcceleration += m_Force * GetInverseMass(); 

	/* �����ӵ��� ����Ѵ� */
	Vec3 angularAcceleration = Vec3::Transform(m_Torque, m_InverseInertiaTensorWorld); 

	/* �ӵ� & ���ӵ��� ������Ʈ�Ѵ� */
	m_Velocity += m_PrevAcceleration * deltaTime;
	m_RotationVelocity += angularAcceleration * deltaTime;

	/* �巡�׸� �����Ѵ� */
	m_Velocity *= powf(m_LinearDamping, deltaTime);
	m_RotationVelocity *= powf(m_AngularDamping, deltaTime);

	/* ȸ�� ������Ʈ�� ���� ����� */
	Quaternion orientation = GetGameObject()->GetTransform()->GetRotationQ();
	Quaternion deltaRotation = Quaternion::CreateFromAxisAngle(m_RotationVelocity, m_RotationVelocity.Length() * deltaTime * 0.5f);
	orientation = Quaternion::Concatenate(orientation, deltaRotation); 

	/* ������� ����ȭ�Ѵ� */
	orientation.Normalize(); 

	/* ������Ʈ �� ȸ���� Ʈ�������� ���� */
	GetGameObject()->GetTransform()->SetRotationQ(orientation); 

	/* ���� ��ǥ�� ������ ���� �ټ��� ������Ʈ�Ѵ� */
	TransformInertiaTensor();

	/* ��ü�� ����� ���� ��ũ�� �����Ѵ� */
	m_Force = Vec3::Zero;
	m_Torque = Vec3::Zero;
} 

void RigidBody::OnDrawGizmos()
{
	Transform* tr = GetGameObject()->GetTransform();
	Gizmo::DrawVector(tr->GetWorldMatrix(), m_Velocity);
	Gizmo::DrawVector(tr->GetWorldMatrix(), m_RotationVelocity);
	//Gizmo::DrawVector(tr->GetWorldMatrix(), m_Force);
	//Gizmo::DrawVector(tr->GetWorldMatrix(), m_Torque);
}

void RigidBody::OnInspectorGUI()
{
	ImGui::DragFloat("Mass", &m_Mass); 
	ImGui::DragFloat("Linear Damping", &m_LinearDamping); 
	ImGui::DragFloat("Angular Damping", &m_AngularDamping);
	ImGui::Checkbox("Is Kinematic", &m_IsKinematic);
}	

GENERATE_COMPONENT_FUNC_TOJSON(RigidBody)
{
	json j = {};
	j["type"] = "RigidBody";
	j["mass"] = m_Mass;
	j["m_LinearDamping"] = m_LinearDamping;
	j["angularDamping"] = m_AngularDamping;
	j["isKinematic"] = m_IsKinematic;

	// Dont save velocity
	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(RigidBody)
{
	m_Mass = j.value("mass", 1.0f);
	m_LinearDamping = j.value("m_LinearDamping", 0.05f);
	m_AngularDamping = j.value("angularDamping", 0.05f);
	m_IsKinematic = j.value("isKinematic", false);

	// Initialize inertia tensor
	m_InverseInertiaTensor = Matrix::Identity;
	m_InverseInertiaTensorWorld = Matrix::Identity;
}
