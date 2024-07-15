#include "pch.h"
#include "RigidBody.h"
#include "Debug.h"

RigidBody::RigidBody()
	: m_Velocity(0.0f, 0.0f, 0.0f),
	m_Acceleration(0.0f, 0.0f, 0.0f),
	m_Mass(1.0f),
	m_InverseMass(0.0f),
	m_Force(Vec3::Zero),
	m_Torque(Vec3::Zero),
	m_IsKinematic(false),
	m_AngularDamping(0.99f),
	m_LinearDamping(0.99f)
{
	m_InspectorTitleName = "RigidBody";
}

RigidBody::~RigidBody()
{
}

// Fixed
void RigidBody::SetInertiaTensor(const Matrix3 mat) 
{
	m_InverseInertiaTensor = mat.inverse(); 
	TransformInertiaTensor();
}

// Fixed
void RigidBody::SetInverseInertiaTensor(const Matrix3& mat)
{
	m_InverseInertiaTensor = mat;
	TransformInertiaTensor();
}

void RigidBody::ApplyTorque(const Vec3& torque)
{
	m_Torque += torque;
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

Vec3 RigidBody::GetRotationVelocity()
{
	Matrix worldMatrix = GetGameObject()->GetTransform()->GetWorldMatrix();
	 
	Matrix3 rotationMatrix;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			rotationMatrix.entries[3 * i + j] = worldMatrix.m[i][j];
	
	// ���� ȸ�� �ӵ��� ���� ȸ�� �ӵ��� ��ȯ
	return rotationMatrix * m_RotationVelocity;
}

// Fixed
void RigidBody::SetRotationVelocity(const Vec3& vec)
{
	Matrix worldMatrix = GetGameObject()->GetTransform()->GetWorldMatrix(); 

	// transformMatrix�� ��� 3x3 �κ��� �����ɴϴ�.
	Matrix3 rotationMatrix;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			rotationMatrix.entries[3 * i + j] = worldMatrix.m[i][j]; 

	// ��ȯ ����� ��ġ ����� ���մϴ�.
	m_RotationVelocity = rotationMatrix.transpose() * vec; 
}

void RigidBody::SetMass(float mass)
{
	m_Mass = mass;

	if (m_Mass > 0) 
		m_InverseMass = 1.0f / m_Mass;
	else
		m_InverseMass = 0;
}

// Fixed
void RigidBody::TransformInertiaTensor()
{
	Matrix worldMatrix = GetGameObject()->GetTransform()->GetWorldMatrix();

	/* ��ȯ ��� �� ȸ�� ��ȯ ��ĸ� �̾Ƴ��� */ 
	Matrix3 rotationMatrix;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			rotationMatrix.entries[3 * i + j] = worldMatrix.m[i][j]; 

	m_InverseInertiaTensorWorld = (rotationMatrix * m_InverseInertiaTensor) * rotationMatrix.transpose();
}

void RigidBody::Integrate(float deltaTime)
{
	if (m_IsKinematic)
		return;

	/* ��ü�� ������ ���Ѵ��� ������ ���� �ʴ´� */
	if (m_InverseMass <= 0.0f)
		return;

	/* ���ӵ��� ����Ѵ� */
	m_PrevAcceleration = m_Acceleration;
	m_PrevAcceleration += m_Force * m_InverseMass;

	/* �����ӵ��� ����Ѵ� */
	Vec3 angularAcceleration = m_InverseInertiaTensorWorld * m_Torque;

	/* �ӵ� & ���ӵ��� ������Ʈ�Ѵ� */
	m_Velocity += m_PrevAcceleration * deltaTime;
	m_RotationVelocity += angularAcceleration * deltaTime;

	/* �巡�׸� �����Ѵ� */
	m_Velocity *= powf(m_LinearDamping, deltaTime);
	m_RotationVelocity *= powf(m_AngularDamping, deltaTime);

	/* ��ġ ������Ʈ */
	GetGameObject()->GetTransform()->Translate(m_Velocity * deltaTime);

	/* ������ ������Ʈ�Ѵ� */
	m_Orientation += RotateByScaledVector(m_Orientation, m_RotationVelocity, deltaTime);   
	m_Orientation.Normalize(); 

	/* ������Ʈ �� ȸ���� Ʈ�������� ���� */
	GetGameObject()->GetTransform()->SetLookRotation(m_Orientation);

	/* ���� ��ǥ�� ������ ���� �ټ��� ������Ʈ�Ѵ� */
	TransformInertiaTensor();

	/* ��ü�� ����� ���� ��ũ�� �����Ѵ� */
	m_Force = Vec3::Zero;
	m_Torque = Vec3::Zero;
}

void RigidBody::Awake()
{
	m_Orientation = GetGameObject()->GetTransform()->GetRotationQ(); 
}


void RigidBody::OnDrawGizmos()
{
	if (m_Mass == 0)
		return;

	Transform* tr = GetGameObject()->GetTransform();
	Gizmo::DrawArrow(tr->GetPosition(), m_Velocity * 0.35f, ImVec4(0, 0, 255, 255));
	Gizmo::DrawArrow(tr->GetPosition(), m_RotationVelocity, ImVec4(0, 255, 0, 255));
}

void RigidBody::OnInspectorGUI()
{
	if (ImGui::DragFloat("Mass", &m_Mass))
	{
		SetMass(m_Mass);
	}
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
	SetMass(j.value("mass", 5.0f)); 
	m_LinearDamping = j.value("m_LinearDamping", 0.05f);
	m_AngularDamping = j.value("angularDamping", 0.05f);
	m_IsKinematic = j.value("isKinematic", false);
}
