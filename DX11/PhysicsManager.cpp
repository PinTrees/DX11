#include "pch.h"
#include "PhysicsManager.h"
#include "Debug.h"
#include "CollisionDetector.h"
#include "CollisionResolver.h"
#include "Debug.h"

// SAT(Separating Axis Theorem)
// GJK / EPA(Gilbert - Johnson - Keerthi / Expanding Polytope Algorithm)

// �浹 �� ����Ǵ� �� ���.
// ħ�� ����(Penetration Depth) �� �浹 ���� ����(Contact Normal) ���.
// Impulse ��� : �������� ���.
// Constraint ��� : LCP Solver ���.
// ��ġ ���� : ħ�� ���̸�ŭ ��ü�� �о�� ��ġ ����.

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

		m_RigidBodies[gameObject->GetInstanceID()] = rigidBody;
		if (box) m_Colliders[gameObject->GetInstanceID()] = box;
		if (sphere) m_Colliders[gameObject->GetInstanceID()] = sphere;

		// ��ü�� ���� ���Ʈ �ټ��� ������ ���� ������
		Matrix3 inertiaTensor;
		if (sphere)
		{
			float value = 0.4f * rigidBody->GetMass();
			inertiaTensor.setDiagonal(Vec3(value, value, value));
		}
		else if (box)
		{
			Vec3 size = box->GetSize(); 
			Transform* transform = box->GetGameObject()->GetTransform();

			size = Vec3(size.x * transform->GetScale().x, size.y * transform->GetScale().y, size.z * transform->GetScale().z);
			size *= 0.15f;

			float mass = rigidBody->GetMass(); 
			float I_x = mass / 12.0f * (size.y * size.y + size.z * size.z);
			float I_y = mass / 12.0f * (size.x * size.x + size.z * size.z);
			float I_z = mass / 12.0f * (size.x * size.x + size.y * size.y);
			inertiaTensor.setDiagonal(Vec3(I_x, I_y, I_z));
		}
		rigidBody->SetAcceleration(Vec3(0.f, -9.8f, 0.f));
		rigidBody->SetInertiaTensor(inertiaTensor); 
	}
}

void PhysicsManager::Update(float deltaTime)
{
	Safe_Delete_Vec(m_Contacts); 

	/* ��ü���� �����Ѵ� */
	for (auto& body : m_RigidBodies)
	{
		body.second->Integrate(deltaTime);
	}

	/* �浹���� ó���Ѵ� */
	m_Detector->DetectCollision(m_Contacts, m_Colliders); 
	m_Resolver->ResolveCollision(m_Contacts, deltaTime);  
}

void PhysicsManager::Exit()
{
	m_RigidBodies.clear();
	m_Colliders.clear();
}

void PhysicsManager::DebugRender()
{
	std::vector<ContactInfo*> contactInfo;
	GetContactInfo(contactInfo);

	for (auto cp : contactInfo)
	{
		Gizmo::DrawArrow(Vec3(cp->pointX, cp->pointY, cp->pointZ), Vec3(cp->normalX, cp->normalY, cp->normalZ));
	}

	Safe_Delete_Vec(contactInfo);
}

void PhysicsManager::GetContactInfo(std::vector<ContactInfo*>& contactInfo) const
{
	for (const auto& contact : m_Contacts)
	{
		for (const auto& cp : contact->contactPoint)
		{
			if (cp == Vec3::Zero)
				continue;

			ContactInfo* newContactInfo = new ContactInfo;
			newContactInfo->pointX = cp.x;
			newContactInfo->pointY = cp.y;
			newContactInfo->pointZ = cp.z;
			newContactInfo->normalX = contact->normal.x;
			newContactInfo->normalY = contact->normal.y;
			newContactInfo->normalZ = contact->normal.z;
			contactInfo.push_back(newContactInfo);
		}
	}
}
