#include "pch.h"
#include "PhysicsManager.h"
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
}

void PhysicsManager::Update(float deltaTime)
{
	/* ��ü���� �����Ѵ� */
	auto gameObjects = SceneManager::GetI()->GetCurrentScene()->GetAllGameObjects();
	for (auto& gameObject : gameObjects)
	{
		auto rigidBody = gameObject->GetComponent<RigidBody>();

		if (rigidBody == nullptr)
			continue;

		rigidBody->Integrate(deltaTime);
	}

	/* �浹���� ó���Ѵ� */
	UpdateCollision();

	ResolveCollision(m_Contacts, deltaTime);
	m_Contacts.clear();
}

void PhysicsManager::UpdateCollision()
{
	m_Contacts.clear();
	mMapColInfo.clear();
	map<ULONGLONG, bool>::iterator iter;

	const std::vector<GameObject*> gameObjects = SceneManager::GetI()->GetCurrentScene()->GetAllGameObjects();

	for (size_t i = 0; i < gameObjects.size(); ++i)
	{
		for (size_t j = 0; j < gameObjects.size(); ++j)
		{
			GameObject* obj1 = gameObjects[i];
			GameObject* obj2 = gameObjects[j];

			// �ڱ� �ڽŰ��� �浹�� ���
			if (obj1->GetInstanceID() == obj2->GetInstanceID())
				continue;

			SphereCollider* sphere1 = obj1->GetComponent<SphereCollider>();
			SphereCollider* sphere2 = obj2->GetComponent<SphereCollider>();
			BoxCollider* box1 = obj1->GetComponent<BoxCollider>();
			BoxCollider* box2 = obj2->GetComponent<BoxCollider>();

			// �� �浹ü ���� ���̵� ����
			COLLIDER_ID colliderID;
			colliderID.Left_id = obj1->GetInstanceID();
			colliderID.RIght_id = obj2->GetInstanceID();

			iter = mMapColInfo.find(colliderID.ID);

			if (mMapColInfo.end() == iter)
			{
				mMapColInfo.insert(make_pair(colliderID.ID, false)); 
				iter = mMapColInfo.find(colliderID.ID); 
			}

			Contact contact;

			if (sphere1 && sphere2)
			{
				if (CheckSphereSphereCollision(sphere1, sphere2, contact))
				{
					HandleCollision(obj1, obj2, colliderID);
				}
			}

			// Sphere-Box collision
			if (sphere1 && box2)
			{
				if (CheckSphereBoxCollision(sphere1, box2, contact))
				{
					HandleCollision(obj1, obj2, colliderID);
				}
			}

			// Sphere-Box collision
			if (box1 && sphere2)
			{
				if (CheckSphereBoxCollision(sphere2, box1, contact))
				{
					HandleCollision(obj1, obj2, colliderID);
				}
			}

			// Box-Box collision
			if (box1 && box2)
			{
				if (CheckBoxBoxCollision(box1, box2, contact))
				{
					HandleCollision(obj1, obj2, colliderID); 
				}
			}
		}
	}
}

float friction = 0.6f;
float objectRestitution = 0.3f;

bool PhysicsManager::CheckSphereSphereCollision(SphereCollider* sphere1, SphereCollider* sphere2, Contact& contact)
{
	Transform* sphere1Tr = sphere1->GetGameObject()->GetTransform();
	Transform* sphere2Tr = sphere2->GetGameObject()->GetTransform();

	RigidBody* sphere1Rb = sphere1->GetGameObject()->GetComponent<RigidBody>();
	RigidBody* sphere2Rb = sphere2->GetGameObject()->GetComponent<RigidBody>();

	/* �� �� ������ �Ÿ��� ���Ѵ� */
	float distanceSquared =
		(sphere1Tr->GetPosition() - sphere2Tr->GetPosition()).LengthSquared();

	/* �� �� ������ �Ÿ��� �� ���� �������� �պ��� �۴ٸ� �浹�� �߻��� ���̴� */
	float radiusSum = sphere1->GetRadius() + sphere2->GetRadius();
	if (distanceSquared < radiusSum * radiusSum)
	{
		Vector3 centerToCenter = sphere1Tr->GetPosition() - sphere2Tr->GetPosition();
		centerToCenter.Normalize();

		/* �浹 ������ �����Ѵ� */
		Contact* newContact = new Contact;
		newContact->bodies[0] = sphere1Rb;
		newContact->bodies[1] = sphere2Rb;
		newContact->normal = centerToCenter;
		newContact->contactPoint[0] = Vector3(sphere1Tr->GetPosition() - centerToCenter * sphere1->GetRadius());
		newContact->contactPoint[1] = Vector3(sphere2Tr->GetPosition() + centerToCenter * sphere2->GetRadius());
		newContact->penetration = radiusSum - sqrtf(distanceSquared);
		newContact->restitution = objectRestitution;
		newContact->friction = friction;
		newContact->normalImpulseSum = 0.0f;
		newContact->tangentImpulseSum1 = 0.0f;
		newContact->tangentImpulseSum2 = 0.0f;

		m_Contacts.push_back(newContact);
		return true;
	}
	else
		return false;
}

bool PhysicsManager::CheckSphereBoxCollision(SphereCollider* sphere, BoxCollider* box, Contact& contact)
{
	Transform* sphereTr = sphere->GetGameObject()->GetTransform();
	Transform* boxTr = box->GetGameObject()->GetTransform();

	/* ���� �߽��� ������ü�� ���� ��ǥ��� ��ȯ�Ѵ� */
	Matrix boxWorld = box->GetGameObject()->GetTransform()->GetWorldMatrix();
	Matrix worldToLocal = box->GetGameObject()->GetTransform()->GetWorldMatrix().Invert(); 
	Vec3 sphereInBoxLocal = Vec3::Transform(sphere->GetGameObject()->GetTransform()->GetPosition(), worldToLocal);
	Vec3 boxHalfSize = box->GetSize() / 2;

	/* ���� �߽ɰ� ���� ����� ������ü ���� ���� ã�´� */
	Vec3 closestPoint;
	/* x �� ���� �� */
	if (sphereInBoxLocal.x > boxHalfSize.x)
		closestPoint.x = boxHalfSize.x;
	else if (sphereInBoxLocal.x < -boxHalfSize.x)
		closestPoint.x = -boxHalfSize.x;
	else
		closestPoint.x = sphereInBoxLocal.x;
	/* y �� ���� �� */
	if (sphereInBoxLocal.y > boxHalfSize.y)
		closestPoint.y = boxHalfSize.y;
	else if (sphereInBoxLocal.y < -boxHalfSize.y)
		closestPoint.y = -boxHalfSize.y;
	else
		closestPoint.y = sphereInBoxLocal.y;
	/* z �� ���� �� */
	if (sphereInBoxLocal.z > boxHalfSize.z)
		closestPoint.z = boxHalfSize.z;
	else if (sphereInBoxLocal.z < -boxHalfSize.z)
		closestPoint.z = -boxHalfSize.z;
	else
		closestPoint.z = sphereInBoxLocal.z;

	/* ���� ����� ���� �߽� ������ �Ÿ��� ���� ���������� �۴ٸ� �浹�� �߻��� ���̴� */
	float distanceSquared = (closestPoint - sphereInBoxLocal).LengthSquared(); 
	if (distanceSquared < sphere->GetRadius() * sphere->GetRadius())
	{
		/* ���� ���� ����� ������ü ���� ���� ���� ��ǥ��� ��ȯ�Ѵ� */
		Vector3 closestPointWorld = Vec3::Transform(closestPoint, boxWorld); 

		/* �浹 ������ �����Ѵ� */
		Contact* newContact = new Contact;
		newContact->bodies[0] = sphere->GetGameObject()->GetComponent<RigidBody>();
		newContact->bodies[1] = box->GetGameObject()->GetComponent<RigidBody>();
		newContact->normal = sphere->GetGameObject()->GetTransform()->GetPosition() - closestPointWorld;
		newContact->normal.Normalize();
		newContact->contactPoint[0] = Vec3(sphereTr->GetPosition() - newContact->normal * sphere->GetRadius());
		newContact->contactPoint[1] = Vec3(closestPointWorld);
		newContact->penetration = sphere->GetRadius() - sqrtf(distanceSquared);
		newContact->restitution = objectRestitution;
		newContact->friction = friction;
		newContact->normalImpulseSum = 0.0f;
		newContact->tangentImpulseSum1 = 0.0f;
		newContact->tangentImpulseSum2 = 0.0f;

		m_Contacts.push_back(newContact);
		return true;
	}
	else
		return false;
}

bool PhysicsManager::CheckBoxBoxCollision(BoxCollider* box1, BoxCollider* box2, Contact& contact)
{
	Transform* box1Tr = box1->GetGameObject()->GetTransform();
	Transform* box2Tr = box2->GetGameObject()->GetTransform();

	RigidBody* box1Rb = box1->GetGameObject()->GetComponent<RigidBody>();
	RigidBody* box2Rb = box2->GetGameObject()->GetComponent<RigidBody>();

	/* ��ħ �˻��� ������ �� �� ���� */
	std::vector<Vector3> axes;

	/* box1 �� �� �� ���� */
	for (int i = 0; i < 3; ++i)
		axes.push_back(box1.body->getAxis(i));

	/* box2 �� �� �� ���� */
	for (int i = 0; i < 3; ++i)
		axes.push_back(box2.body->getAxis(i));

	/* �� �� ������ ���� ���� */
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			Vector3 crossProduct = axes[i].Cross(axes[3 + j]);
			crossProduct.Normalize();
			axes.push_back(crossProduct);
		}
	}

	float minPenetration = FLT_MAX;
	int minAxisIdx = 0;

	/* ��� �࿡ ���� ��ħ �˻� */
	for (int i = 0; i < axes.size(); ++i)
	{
		float penetration = calcPenetration(box1, box2, axes[i]);

		/* �� ���̶� ��ġ�� ������ �浹�� �߻����� ���� ���̴� */
		if (penetration <= 0.0f)
			return false;

		/* ���� ���� ��ġ�� ������ �׶��� ���� ���� �����Ѵ� */
		if (penetration <= minPenetration)
		{
			minPenetration = penetration;
			minAxisIdx = i;
		}
	}

	/* ��� �࿡ ���� ��ħ�� �����ƴٸ� �浹�� �߻��� ���̴� */
	Contact* newContact = new Contact;
	newContact->bodies[0] = box1Rb;
	newContact->bodies[1] = box2Rb;
	newContact->penetration = minPenetration;
	newContact->restitution = objectRestitution;
	newContact->friction = friction;
	newContact->normalImpulseSum = 0.0f;
	newContact->tangentImpulseSum1 = 0.0f;
	newContact->tangentImpulseSum2 = 0.0f;

	/* �浹 ������ ���⿡ �����Ͽ� �����Ѵ� */
	Vector3 centerToCenter = box1Tr->GetPosition() - box2Tr->GetPosition();
	if (axes[minAxisIdx].Dot(centerToCenter) > 0)
		newContact->normal = axes[minAxisIdx] * -1.0f;
	else
		newContact->normal = axes[minAxisIdx];

	/* �浹 ������ ã�´� */
	if (minAxisIdx < 6) // ��-�� ������ ��
	{
		calcContactPointOnPlane(box1, box2, minAxisIdx, newContact);
	}
	else // ��-�� ������ ��
	{
		calcContactPointOnLine(box1, box2, minAxisIdx, newContact);
	}

	m_Contacts.push_back(newContact);
	return true;
}

void PhysicsManager::HandleCollision(GameObject* obj1, GameObject* obj2, COLLIDER_ID colliderID)
{
	if (mMapColInfo.find(colliderID.ID) == mMapColInfo.end())
	{
		// First time collision in this frame, handle collision
		// (collision response logic goes here)
		mMapColInfo[colliderID.ID] = true;
	}
}

void PhysicsManager::ResolveCollision(std::vector<Contact*>& contacts, float deltaTime)
{
	for (int i = 0; i < 10; ++i)
	{
		for (auto& contact : contacts)
		{
			sequentialImpulse(contact, deltaTime);
		}
	}
}

float penetrationTolerance = 0.0005f;
float closingSpeedTolerance = 0.005f;

void PhysicsManager::sequentialImpulse(Contact* contact, float deltaTime)
{
	float effectiveMass;

	float totalInvMass = contact->bodies[0]->GetInverseMass();
	if (contact->bodies[1] != nullptr)
		totalInvMass += contact->bodies[1]->GetInverseMass();
	if (totalInvMass == 0.0f)
		return;

	Vec3 contactPointFromCenter1 = contact->contactPoint[0] - contact->bodies[0]->GetGameObject()->GetTransform()->GetPosition();
	Vector3 contactPointFromCenter2;
	if (contact->bodies[1] != nullptr)
		contactPointFromCenter2 = contact->contactPoint[1] - contact->bodies[1]->GetGameObject()->GetTransform()->GetPosition();

	Vec3 termInDenominator1 = Vec3::Transform(contactPointFromCenter1.Cross(contact->normal), contact->bodies[0]->GetInverseInertiaTensorWorld())
		.Cross(contactPointFromCenter1);

	Vec3 termInDenominator2;
	if (contact->bodies[1] != nullptr) {
		termInDenominator2 = Vec3::Transform(contactPointFromCenter2.Cross(contact->normal), contact->bodies[1]->GetInverseInertiaTensorWorld())
			.Cross(contactPointFromCenter2); 
	}
	effectiveMass = totalInvMass + (termInDenominator1 + termInDenominator2).Dot(contact->normal);
	if (effectiveMass == 0.0f)
		return;

	float closingSpeed = contact->normal.Dot(contact->bodies[0]->GetVelocity())
		+ contact->bodies[0]->GetGameObject()->GetTransform()->GetRotation().Dot(contactPointFromCenter1.Cross(contact->normal));
	if (contact->bodies[1] != nullptr)
	{
		closingSpeed -= contact->normal.Dot(contact->bodies[1]->GetVelocity())
			+ contact->bodies[1]->GetGameObject()->GetTransform()->GetRotation().Dot(contactPointFromCenter2.Cross(contact->normal));
	}

	/* bias ��� */
	float bias = 0.0f;
	/* Baumgarte Stabilization */
	float baumgarte = 0.0f;
	if (contact->penetration > penetrationTolerance)
		baumgarte = (-0.1f / deltaTime) * (contact->penetration - penetrationTolerance);
	bias += baumgarte;

	/* bias �� restitution term �߰� */
	float restitutionTerm = 0.0f;
	if (closingSpeed > closingSpeedTolerance)
		restitutionTerm = contact->restitution * (closingSpeed - closingSpeedTolerance);
	bias -= restitutionTerm;

	float impulse = -(closingSpeed + bias) / effectiveMass;
	if (isnan(impulse) != 0)
	{
		std::cout << "ERROR::CollisionResolver::sequentialImpulse()::impulse is nan" << std::endl;
		return;
	}

	/* ��ݷ��� �������� clamp */
	float prevImpulseSum = contact->normalImpulseSum; 
	contact->normalImpulseSum += impulse; 
	if (contact->normalImpulseSum < 0.0f) 
		contact->normalImpulseSum = 0.0f; 
	impulse = contact->normalImpulseSum - prevImpulseSum;

	/* �ӵ� & ���ӵ� ��ȭ */
	Vector3 linearImpulse = contact->normal * impulse;
	Vector3 angularImpulse1 = contactPointFromCenter1.Cross(contact->normal) * impulse;
	Vector3 angularImpulse2 = contactPointFromCenter2.Cross(contact->normal) * impulse;

	contact->bodies[0]->SetVelocity(
		contact->bodies[0]->GetVelocity() + linearImpulse * contact->bodies[0]->GetInverseMass()
	);
	contact->bodies[0]->GetGameObject()->GetTransform()->SetRotation( 
		contact->bodies[0]->GetGameObject()->GetTransform()->GetRotation() + Vector3::Transform(angularImpulse1, contact->bodies[0]->GetInverseInertiaTensorWorld())
	); 

	if (contact->bodies[1] != nullptr)
	{
		contact->bodies[1]->SetVelocity(
			contact->bodies[1]->GetVelocity() - linearImpulse * contact->bodies[1]->GetInverseMass()
		);
		contact->bodies[1]->GetGameObject()->GetTransform()->SetRotation(
			contact->bodies[1]->GetGameObject()->GetTransform()->GetRotation() - Vector3::Transform(angularImpulse2, contact->bodies[1]->GetInverseInertiaTensorWorld())
		);
	}

	/* �浹 ������ �����ϴ� ���� ã��(erin catto ���) */
	Vector3 tangent1, tangent2;
	if (contact->normal.x >= 0.57735f)
		tangent1 = Vector3(contact->normal.y, -contact->normal.x, 0.0f);
	else
		tangent1 = Vector3(0.0f, contact->normal.z, -contact->normal.y);
	tangent2 = contact->normal.Cross(tangent1);

	/* tangent1 ���Ϳ� ���� ���� ��� */
	Vec3 termInDenominator1 = Vector3::Transform(contactPointFromCenter1.Cross(tangent1), contact->bodies[0]->GetInverseInertiaTensorWorld()).Cross(contactPointFromCenter1);
	if (contact->bodies[1] != nullptr) {
		termInDenominator2 = Vector3::Transform(contactPointFromCenter2.Cross(tangent1), contact->bodies[1]->GetInverseInertiaTensorWorld()).Cross(contactPointFromCenter2);
	}

	effectiveMass = totalInvMass + (termInDenominator1 + termInDenominator2).Dot(tangent1);

	closingSpeed = tangent1.Dot(contact->bodies[0]->GetVelocity())
		+ contact->bodies[0]->GetGameObject()->GetTransform()->GetRotation().Dot(contactPointFromCenter1.Cross(tangent1));
	if (contact->bodies[1] != nullptr)
	{
		closingSpeed -= tangent1.Dot(contact->bodies[1]->GetVelocity())
			+ contact->bodies[1]->GetGameObject()->GetTransform()->GetRotation().Dot(contactPointFromCenter2.Cross(tangent1));
	}
	impulse = -closingSpeed / effectiveMass;
	if (isnan(impulse) != 0)
	{
		std::cout << "ERROR::CollisionResolver::sequentialImpulse()::tangential impulse1 is nan" << std::endl;
		return;
	}

	/* ��ݷ��� �������� clamp */
	prevImpulseSum = contact->tangentImpulseSum1;
	contact->tangentImpulseSum1 += impulse;
	if (contact->tangentImpulseSum1 < (-contact->friction * contact->normalImpulseSum))
		contact->tangentImpulseSum1 = -contact->friction * contact->normalImpulseSum;
	else if (contact->tangentImpulseSum1 > (contact->friction * contact->normalImpulseSum))
		contact->tangentImpulseSum1 = contact->friction * contact->normalImpulseSum;
	impulse = contact->tangentImpulseSum1 - prevImpulseSum;

	/* �ӵ� & ���ӵ� ��ȭ */
	linearImpulse = tangent1 * impulse;
	angularImpulse1 = contactPointFromCenter1.Cross(tangent1) * impulse;
	angularImpulse2 = contactPointFromCenter2.Cross(tangent1) * impulse;

	contact->bodies[0]->SetVelocity(
		contact->bodies[0]->GetVelocity() + linearImpulse * contact->bodies[0]->GetInverseMass()
	);
	contact->bodies[0]->GetGameObject()->GetTransform()->SetRotation(
    contact->bodies[0]->GetGameObject()->GetTransform()->GetRotation() + Vector3::Transform(angularImpulse1, contact->bodies[0]->GetInverseInertiaTensorWorld())
);
	if (contact->bodies[1] != nullptr)
	{
		contact->bodies[1]->SetVelocity(
			contact->bodies[1]->GetVelocity() - linearImpulse * contact->bodies[1]->GetInverseMass()
		);
		contact->bodies[1]->GetGameObject()->GetTransform()->SetRotation(
			contact->bodies[1]->GetGameObject()->GetTransform()->GetRotation() - Vec3::Transform(angularImpulse2, contact->bodies[1]->GetInverseInertiaTensorWorld())
		);
	}

	/* tangent2 ���Ϳ� ���� ���� ��� */
	termInDenominator1 = Vec3::Transform(contactPointFromCenter1.Cross(tangent2), contact->bodies[0]->GetInverseInertiaTensorWorld()).Cross(contactPointFromCenter1);
	if (contact->bodies[1] != nullptr) {
		termInDenominator2 = Vec3::Transform(contactPointFromCenter2.Cross(tangent2),
			contact->bodies[1]->GetInverseInertiaTensorWorld()).Cross(contactPointFromCenter2);
	}
	effectiveMass = totalInvMass + (termInDenominator1 + termInDenominator2).Dot(tangent2);

	closingSpeed = tangent2.Dot(contact->bodies[0]->GetVelocity())
		+ contact->bodies[0]->GetGameObject()->GetTransform()->GetRotation().Dot(contactPointFromCenter1.Cross(tangent2));
	if (contact->bodies[1] != nullptr)
	{
		closingSpeed -= tangent2.Dot(contact->bodies[1]->GetVelocity())
			+ contact->bodies[1]->GetGameObject()->GetTransform()->GetRotation().Dot(contactPointFromCenter2.Cross(tangent2));
	}
	impulse = -closingSpeed / effectiveMass;
	if (isnan(impulse) != 0)
	{
		std::cout << "ERROR::CollisionResolver::sequentialImpulse()::tangential impulse2 is nan" << std::endl;
		return;
	}

	/* ��ݷ��� �������� clamp */
	prevImpulseSum = contact->tangentImpulseSum2;
	contact->tangentImpulseSum2 += impulse;
	if (contact->tangentImpulseSum2 < (-contact->friction * contact->normalImpulseSum))
		contact->tangentImpulseSum2 = -contact->friction * contact->normalImpulseSum;
	else if (contact->tangentImpulseSum2 > (contact->friction * contact->normalImpulseSum))
		contact->tangentImpulseSum2 = contact->friction * contact->normalImpulseSum;
	impulse = contact->tangentImpulseSum2 - prevImpulseSum;

	/* �ӵ� & ���ӵ� ��ȭ */
	linearImpulse = tangent2 * impulse;
	angularImpulse1 = contactPointFromCenter1.Cross(tangent2) * impulse;
	angularImpulse2 = contactPointFromCenter2.Cross(tangent2) * impulse;

	contact->bodies[0]->SetVelocity(
		contact->bodies[0]->GetVelocity() + linearImpulse * contact->bodies[0]->GetInverseMass()
	);
	contact->bodies[0]->SetRotation(
		contact->bodies[0]->GetRotation() + Vec3::Transform(angularImpulse1, contact->bodies[0]->GetInverseInertiaTensorWorld())
	);
	if (contact->bodies[1] != nullptr)
	{
		contact->bodies[1]->SetVelocity(
			contact->bodies[1]->GetVelocity() - linearImpulse * contact->bodies[1]->GetInverseMass()
		);
		contact->bodies[1]->SetRotation(
			contact->bodies[1]->GetRotation() - Vec3::Transform(angularImpulse2, contact->bodies[1]->GetInverseInertiaTensorWorld())
		);
	}
}



