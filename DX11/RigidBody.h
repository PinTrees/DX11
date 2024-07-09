#pragma once
#include "Component.h"

class Transform;

class RigidBody
	: public Component
{
private:
	float m_Mass;						// 질량
	//float m_InverseMass;				// 질량의 역수. - 적분식에 활용

	/* 관성 모멘트 역텐서 */
	Matrix m_InverseInertiaTensor;		// 역회전관성 로컬 좌표계 기준
	Matrix m_InverseInertiaTensorWorld;	// 역회전관성 월드 좌표계 기준

	Vec3 m_Velocity;				// 선속도
	Vec3 m_RotationVelocity;		// 각속도

	Vec3 m_Acceleration;			// 가속도
	Vec3 m_PrevAcceleration;		// 직전 프레임에서 강체의 가속도 - 중력 가속도 + 외부 힘에 의한 가속도를 저장한다

	float m_LinearDamping;			// 선속도 댐핑
	float m_AngularDamping;			// 각속도 댐핑

	/* 강체에 가해진 힘 & 토크 */
	Vec3 m_Force;
	Vec3 m_Torque;
	
	bool m_IsKinematic;				// 키네틱 여부
	Matrix m_InertiaTensor;			// 회전관성		(Inertia Tensor)

	Vec3 m_CenterOfMass;			// 무게 중심

public:
	RigidBody();
	~RigidBody();

public:
	/* getter */
	Matrix GetInverseInertiaTensor() const { return m_InverseInertiaTensor; };
	Matrix GetInverseInertiaTensorWorld() const { return m_InverseInertiaTensorWorld; };
	
	void SetKinematic(bool isKinematic) { m_IsKinematic = isKinematic; }
	bool IsKinematic() const { return m_IsKinematic; }

	void SetVelocity(const Vec3& velocity) { m_Velocity = velocity; }
	Vec3 GetVelocity() const { return m_Velocity; }

	void SetMass(float mass) { m_Mass = mass; }
	float GetMass() const { return m_Mass; }

	Vec3 GetCenterOfMass();

	float GetInverseMass() const { return (m_Mass == 0) ? 0 : 1.0f / m_Mass; } 
	//float GetRestitution() const { return m_Elasticity; } 
	//float GetFriction() const { return m_Friction; }

	void ApplyTorque(const Vec3& torque);
	void ApplyForce(const Vec3& force);
	void ApplyImpulse(const Vec3& impulse);
	
	// 오버라이드
	Vec3 GetRotation();
	void SetRotation(const Vec3& rotation);
	void TransformInertiaTensor();

	void Integrate(float deltaTime);

public:
	virtual void OnDrawGizmos() override;
	virtual void OnInspectorGUI() override;

	GENERATE_COMPONENT_BODY(RigidBody) 
};

REGISTER_COMPONENT(RigidBody); 

