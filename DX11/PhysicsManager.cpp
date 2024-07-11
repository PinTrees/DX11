#include "pch.h"
#include "PhysicsManager.h"
#include "Debug.h"
#include "CollisionDetector.h"
#include "CollisionResolver.h"

// SAT(Separating Axis Theorem)
// GJK / EPA(Gilbert - Johnson - Keerthi / Expanding Polytope Algorithm)

// 충돌 시 적용되는 힘 계산.
// 침투 깊이(Penetration Depth) 및 충돌 법선 벡터(Contact Normal) 계산.
// Impulse 기반 : 직관적인 방식.
// Constraint 기반 : LCP Solver 사용.
// 위치 보정 : 침투 깊이만큼 물체를 밀어내는 위치 보정.

SINGLE_BODY(PhysicsManager)

PhysicsManager::PhysicsManager()
	: m_GravityAcceleration(9.8f)
{

}

PhysicsManager::~PhysicsManager()
{

}

void PhysicsManager::Init()
{
	SetGravity(9.8f);

	m_Detector = new CollisionDetector; 
	m_Resolver = new CollisionResolver;
}

void PhysicsManager::Start()
{
	auto gameObjects = SceneManager::GetI()->GetCurrentScene()->GetAllGameObjects();
	for (auto& gameObject : gameObjects)
	{
		auto rigidBody = gameObject->GetComponent<RigidBody>();

		if (rigidBody == nullptr)
			continue;

		BoxCollider* box = gameObject->GetComponent<BoxCollider>();
		SphereCollider* sphere = gameObject->GetComponent<SphereCollider>();

		// 강체의 관성 모멘트 텐서를 도형에 따라 설정함
		Matrix inertiaTensor = Matrix::Identity;
		if (sphere)
		{
			float value = 0.4f * rigidBody->GetMass();
			inertiaTensor._11 = value;
			inertiaTensor._22 = value;
			inertiaTensor._33 = value;
		}
		else if (box)
		{
			float value = rigidBody->GetMass() / 6.0f;
			inertiaTensor._11 = value;
			inertiaTensor._22 = value;
			inertiaTensor._33 = value;
		}
		rigidBody->SetInertiaTensor(inertiaTensor);
	}
}

void PhysicsManager::Update(float deltaTime)
{
	/* 물체들을 적분한다 */
	auto gameObjects = SceneManager::GetI()->GetCurrentScene()->GetAllGameObjects();
	for (auto& gameObject : gameObjects)
	{
		auto rigidBody = gameObject->GetComponent<RigidBody>();

		if (rigidBody == nullptr)
			continue;

		rigidBody->Integrate(deltaTime);
	}

	/* 충돌들을 처리한다 */
	m_Detector->DetectCollision(m_Contacts, gameObjects);
	m_Resolver->ResolveCollision(m_Contacts, deltaTime);
	Safe_Delete_Vec(m_Contacts);
}
