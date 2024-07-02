#include "pch.h"
#include "PhysicsManager.h"
#include "RigidBody.h"

SINGLE_BODY(PhysicsManager)

PhysicsManager::PhysicsManager()
	: m_GravityAcceleration(9.8f)
{

}

PhysicsManager::~PhysicsManager()
{

}

void PhysicsManager::Update(float deltaTime)
{
	const vector<GameObject*> gameObjects = SceneManager::GetI()->GetCurrentScene()->GetAllGameObjects();

	for (auto& gameObject : gameObjects)
	{
		auto rigidBody = gameObject->GetComponent<RigidBody>();

		if (rigidBody == nullptr)
			continue;

		//rigidBody->velocity.y += m_gravityAcceleration * deltaTime;
		//gameObject->position.y += rigidBody->velocity.y * deltaTime + 0.5f * m_gravityAcceleration * deltaTime * deltaTime;
	}
}
