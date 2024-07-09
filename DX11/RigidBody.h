#pragma once
#include "Component.h"

class Transform;

// 질량: F = ma에서 가속도를 구하는 데 필요.
// 선속도 : 가속도와 시간으로 업데이트.
// 회전관성(Inertia Tensor) : 회전 동역학에서 사용.
// 각속도 : 각가속도와 시간으로 업데이트.
// 무게 중심(Center of Mass) : 강체의 위치 표현.
// 로컬 좌표계 : x, y, z 축(강체의 회전 상태를 나타냄).
// 충돌 체크를 위한 경계 도형(AABB, OBB, Sphere 등).
// 상태 행렬 : 강체의 위치 및 회전을 포함.
// 추가 요소 : 공기저항, 마찰력, 탄성계수, 운동에너지 등.	

class RigidBody
	: public Component
{
private:
	Vec3 m_Velocity;				// 선속도
	Vec3 m_Acceleration;			// 가속도
	Vec3 m_AngularVelocity;			// 각속도
	Vec3 m_AngularAcceleration;		// 각가속도
	float m_Mass;					// 질량
	float m_Elasticity;				// 반발 계수
	float m_AirResistance;			// 공기저항
	float m_Friction;				// 마찰력
	bool m_IsKinematic;				// 키네틱 여부
	Matrix m_InertiaTensor;			// 회전관성		(Inertia Tensor)
	Matrix m_InverseInertiaTensor;	// 역회전관성

	Vec3 m_CenterOfMass;			// 무게 중심

	bool m_IsMovable;				// 움직일 수 있는가
	float m_Restitution;			// 탄성계수
	bool m_Sleep;					// Sleep 상태

public:
	RigidBody();
	~RigidBody();

public:
	void SetKinematic(bool isKinematic) { m_IsKinematic = isKinematic; }
	bool IsKinematic() const { return m_IsKinematic; }

	void SetVelocity(const Vec3& velocity) { m_Velocity = velocity; }
	Vec3 GetVelocity() const { return m_Velocity; }

	void SetMass(float mass) { m_Mass = mass; }
	float GetMass() const { return m_Mass; }

	void SetElasticity(float elasticity) { m_Elasticity = elasticity; }
	float GetInverseMass() const { return (m_Mass == 0) ? 0 : 1.0f / m_Mass; }
	float GetRestitution() const { return m_Elasticity; }
	float GetFriction() const { return m_Friction; }

	void ApplyTorque(const Vec3& torque);
	void ApplyForce(const Vec3& force);
	void ApplyImpulse(const Vec3& impulse);
	

	void Integrate(float deltaTime);

public:
	virtual void OnDrawGizmos() override;
	virtual void OnInspectorGUI() override;

	GENERATE_COMPONENT_BODY(RigidBody) 
};

REGISTER_COMPONENT(RigidBody); 

