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

	// 초기화 시 회전관성 텐서와 그 역행렬을 설정
	m_InverseInertiaTensor = Matrix::Identity;
	m_CenterOfMass = Vec3::Zero;
}

RigidBody::~RigidBody()
{
}

void RigidBody::SetInertiaTensor(Matrix mat)
{
	m_InverseInertiaTensor = mat.Invert(); 
	TransformInertiaTensor(); 
}

Vec3 RigidBody::GetCenterOfMass()
{
	return GetGameObject()->GetTransform()->GetPosition();
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
	return m_RotationVelocity;
}

void RigidBody::SetRotationVelocity(const Vec3& vec)
{
	Transform* tr = GetGameObject()->GetTransform(); 
	Matrix worldMatrix = tr->GetWorldMatrix(); 

	// transformMatrix의 상단 3x3 부분을 가져옵니다.
	Matrix rotationMatrix( 
		worldMatrix._11, worldMatrix._12, worldMatrix._13, 0.0f,
		worldMatrix._21, worldMatrix._22, worldMatrix._23, 0.0f,
		worldMatrix._31, worldMatrix._32, worldMatrix._33, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	); 

	// 변환 행렬의 전치 행렬을 곱합니다.
	m_RotationVelocity = Vector3::Transform(vec, rotationMatrix.Transpose()); 
}

void RigidBody::TransformInertiaTensor()
{
	Matrix worldMatrix = GetGameObject()->GetTransform()->GetWorldMatrix();
	Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(
		GetGameObject()->GetTransform()->GetRotation().y,
		GetGameObject()->GetTransform()->GetRotation().x,
		GetGameObject()->GetTransform()->GetRotation().z
	);
	m_InverseInertiaTensorWorld = rotationMatrix * m_InverseInertiaTensor * rotationMatrix.Transpose();
}

void RigidBody::Integrate(float deltaTime)
{
	if (m_IsKinematic)
		return;

	/* 강체의 질량이 무한대라면 적분을 하지 않는다 */
	if (GetInverseMass() == 0.0f)
		return;

	/* 가속도를 계산한다 */
	m_PrevAcceleration = m_Acceleration;
	m_PrevAcceleration += m_Force * GetInverseMass();

	/* 각가속도를 계산한다 */
	Vec3 angularAcceleration = Vec3::Transform(m_Torque, m_InverseInertiaTensorWorld);

	/* 속도 & 각속도를 업데이트한다 */
	m_Velocity += m_PrevAcceleration * deltaTime;
	m_RotationVelocity += angularAcceleration * deltaTime;

	/* 드래그를 적용한다 */
	m_Velocity *= powf(m_LinearDamping, deltaTime);
	m_RotationVelocity *= powf(m_AngularDamping, deltaTime);

	/* 위치 업데이트 */
	GetGameObject()->GetTransform()->Translate(m_Velocity * deltaTime);

	/* 회전 업데이트를 위한 사원수 */
	Quaternion orientation = GetGameObject()->GetTransform()->GetRotationQ();
	if (m_RotationVelocity.LengthSquared() > FLT_EPSILON)
	{
		Quaternion deltaRotation = Quaternion::CreateFromAxisAngle(m_RotationVelocity, m_RotationVelocity.Length() * deltaTime);
		orientation = Quaternion::Concatenate(orientation, deltaRotation);
	}

	//m_Orientation += m_Orientation.RotateByScaledVector(rotation, duration / 2.0f);

	/* 사원수를 정규화한다 */
	orientation.Normalize();

	/* 업데이트 된 회전을 트랜스폼에 설정 */
	GetGameObject()->GetTransform()->SetRotationQ(orientation);

	/* 월드 좌표계 기준의 관성 텐서를 업데이트한다 */
	TransformInertiaTensor();

	/* 강체에 적용된 힘과 토크는 제거한다 */
	m_Force = Vec3::Zero;
	m_Torque = Vec3::Zero;
} 

void RigidBody::OnDrawGizmos()
{
	Transform* tr = GetGameObject()->GetTransform();
	//Gizmo::DrawVector(tr->GetWorldMatrix(), m_Velocity);
	//Gizmo::DrawVector(tr->GetWorldMatrix(), m_RotationVelocity);
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
	m_Mass = j.value("mass", 5.0f);
	m_LinearDamping = j.value("m_LinearDamping", 0.05f);
	m_AngularDamping = j.value("angularDamping", 0.05f);
	m_IsKinematic = j.value("isKinematic", false);

	// Initialize inertia tensor
	m_InverseInertiaTensor = Matrix::Identity;
	m_InverseInertiaTensorWorld = Matrix::Identity;

	m_Acceleration = Vec3(0.0f, -9.8f, 0.0f);

	if (m_Mass == 0) m_Mass = 5.0f;
}
