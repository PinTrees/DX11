#include "pch.h"
#include "CollisionDetector.h"

CollisionDetector::CollisionDetector()
	: friction(0.6f),
	objectRestitution(0.3f),
	groundRestitution(0.2f) 
{
}

void CollisionDetector::DetectCollision(std::vector<Contact*>& contacts, std::unordered_map<unsigned int, Collider*>& colliders)
{
	for (auto i = colliders.begin(); i != colliders.end(); ++i)
	{
		Collider* colliderPtrI = i->second;
		for (auto j = std::next(i, 1); j != colliders.end(); ++j)
		{
			Collider* colliderPtrJ = j->second;
			if (typeid(*colliderPtrI) == typeid(SphereCollider))
			{
				SphereCollider* collider1 = static_cast<SphereCollider*>(colliderPtrI);
				if (typeid(*colliderPtrJ) == typeid(SphereCollider)) // �� - �� �浹
				{
					SphereCollider* collider2 = static_cast<SphereCollider*>(colliderPtrJ);
					CheckSphereSphereCollision(contacts, collider1, collider2);
				}
				else if (typeid(*colliderPtrJ) == typeid(BoxCollider)) // �� - ������ü �浹
				{
					BoxCollider* collider2 = static_cast<BoxCollider*>(colliderPtrJ);
					CheckSphereBoxCollision(contacts, collider1, collider2);
				}
			}
			else if (typeid(*colliderPtrI) == typeid(BoxCollider))
			{
				BoxCollider* collider1 = static_cast<BoxCollider*>(colliderPtrI);
				if (typeid(*colliderPtrJ) == typeid(SphereCollider)) // �� - ������ü �浹
				{
					SphereCollider* collider2 = static_cast<SphereCollider*>(colliderPtrJ);
					CheckSphereBoxCollision(contacts, collider2, collider1);
				}
				else if (typeid(*colliderPtrJ) == typeid(BoxCollider)) // ������ü - ������ü �浹
				{
					BoxCollider* collider2 = static_cast<BoxCollider*>(colliderPtrJ);
					CheckBoxBoxCollision(contacts, collider1, collider2);
				}
			}
		}

		/* ������� �浹 �˻� */
	}
}


bool CollisionDetector::CheckSphereSphereCollision(std::vector<Contact*>& contacts, SphereCollider* sphere1, SphereCollider* sphere2)
{
	Transform* sphere1Tr = sphere1->GetGameObject()->GetTransform();
	Transform* sphere2Tr = sphere2->GetGameObject()->GetTransform();

	RigidBody* sphere1Rb = sphere1->GetGameObject()->GetComponent<RigidBody>();
	RigidBody* sphere2Rb = sphere2->GetGameObject()->GetComponent<RigidBody>();

	float sphere1Radius = sphere1->GetRadius() * sphere1Tr->GetScale().x;
	float sphere2Radius = sphere2->GetRadius() * sphere2Tr->GetScale().x;

	/* �� �� ������ �Ÿ��� ���Ѵ� */
	float distanceSquared =
		(sphere1Tr->GetPosition() - sphere2Tr->GetPosition()).LengthSquared();

	/* �� �� ������ �Ÿ��� �� ���� �������� �պ��� �۴ٸ� �浹�� �߻��� ���̴� */
	float radiusSum = sphere1Radius + sphere2Radius;
	if (distanceSquared < radiusSum * radiusSum)
	{
		Vector3 centerToCenter = sphere1Tr->GetPosition() - sphere2Tr->GetPosition();
		centerToCenter.Normalize();

		/* �浹 ������ �����Ѵ� */
		Contact* newContact = new Contact;
		newContact->bodies[0] = sphere1Rb;
		newContact->bodies[1] = sphere2Rb;
		newContact->normal = centerToCenter;
		newContact->contactPoint[0] = Vector3(sphere1Tr->GetPosition() - centerToCenter * sphere1Radius);
		newContact->contactPoint[1] = Vector3(sphere2Tr->GetPosition() + centerToCenter * sphere2Radius);
		newContact->penetration = radiusSum - sqrtf(distanceSquared);
		newContact->restitution = objectRestitution;
		newContact->friction = friction;
		newContact->normalImpulseSum = 0.0f;
		newContact->tangentImpulseSum1 = 0.0f;
		newContact->tangentImpulseSum2 = 0.0f;

		contacts.push_back(newContact);
		return true;
	}
	else
		return false;
}

// Fixed 
// ���� �ڽ����� ���� ������ ���� ���� - �ڽ��� �������� ���� ��� ���� �������� �ſ� Ŀ���� ���� ����
bool CollisionDetector::CheckSphereBoxCollision(std::vector<Contact*>& contacts, SphereCollider* sphere, BoxCollider* box)
{
	Transform* sphereTr = sphere->GetGameObject()->GetTransform(); 
	Transform* boxTr = box->GetGameObject()->GetTransform(); 

	RigidBody* sphereRb = sphere->GetGameObject()->GetComponent<RigidBody>(); 
	RigidBody* boxRb = box->GetGameObject()->GetComponent<RigidBody>(); 

	/* ���� �߽��� ������ü�� ���� ��ǥ��� ��ȯ�Ѵ� */
	Matrix sphereInBoxLocalMatrix = sphereTr->GetWorldMatrix() * boxTr->GetWorldMatrix().Invert();
	Vec3 sphereInBoxLocalPosition = Vec3(sphereInBoxLocalMatrix._41, sphereInBoxLocalMatrix._42, sphereInBoxLocalMatrix._43); 
	Vec3 sphereInBoxLocalScale = Vec3(
		Vec3(sphereInBoxLocalMatrix._11, sphereInBoxLocalMatrix._12, sphereInBoxLocalMatrix._13).Length(),
		Vec3(sphereInBoxLocalMatrix._21, sphereInBoxLocalMatrix._22, sphereInBoxLocalMatrix._23).Length(),
		Vec3(sphereInBoxLocalMatrix._31, sphereInBoxLocalMatrix._32, sphereInBoxLocalMatrix._33).Length());

	// �ڽ��� ���� ũ��
	Vec3 boxHalfSize = box->GetSize() * 0.5f;
	// ���� �ڽ���ǥ�� ���� ������
	float sphereRadius = sphere->GetRadius() * min(sphereInBoxLocalScale.x, min(sphereInBoxLocalScale.y, sphereInBoxLocalScale.z))
		* max(boxTr->GetScale().x, max(boxTr->GetScale().y, boxTr->GetScale().z));
	// ���� ������ǥ ���� ������
	float sphereOriginal = sphere->GetRadius() * sphereTr->GetScale().x;

	/* ���� �߽ɰ� ���� ����� ������ü ���� ���� ã�´� */
	Vector3 closestPoint;
	/* x �� ���� �� */
	if (sphereInBoxLocalPosition.x > boxHalfSize.x)
		closestPoint.x = boxHalfSize.x;
	else if (sphereInBoxLocalPosition.x < -boxHalfSize.x)
		closestPoint.x = -boxHalfSize.x;
	else
		closestPoint.x = sphereInBoxLocalPosition.x;

	/* y �� ���� �� */
	if (sphereInBoxLocalPosition.y > boxHalfSize.y)
		closestPoint.y = boxHalfSize.y;
	else if (sphereInBoxLocalPosition.y < -boxHalfSize.y)
		closestPoint.y = -boxHalfSize.y;
	else
		closestPoint.y = sphereInBoxLocalPosition.y;

	/* z �� ���� �� */
	if (sphereInBoxLocalPosition.z > boxHalfSize.z)
		closestPoint.z = boxHalfSize.z;
	else if (sphereInBoxLocalPosition.z < -boxHalfSize.z)
		closestPoint.z = -boxHalfSize.z;
	else
		closestPoint.z = sphereInBoxLocalPosition.z;

	/* ���� ���� ����� ������ü ���� ���� ���� ��ǥ��� ��ȯ�Ѵ� */
	Vec3 closestPointWorld = Vec3::Transform(closestPoint, boxTr->GetWorldMatrix()); 

	/* ���� ����� ���� �߽� ������ �Ÿ��� ���� ���������� �۴ٸ� �浹�� �߻��� ���̴� */
	// ������ ����� ���� ������ǥ�� ����մϴ�.
	float distanceSquared = (closestPointWorld - sphereTr->GetPosition()).LengthSquared(); 
	if (distanceSquared < sphereRadius * sphereRadius) 
	{
		/* �浹 ������ �����Ѵ� */
		Contact* newContact = new Contact;
		newContact->bodies[0] = sphereRb;
		newContact->bodies[1] = boxRb;
		newContact->normal = sphereTr->GetPosition() - closestPointWorld;
		newContact->normal.Normalize();
		newContact->contactPoint[0] = Vec3(sphereTr->GetPosition() - newContact->normal * sphereOriginal);
		newContact->contactPoint[1] = Vec3(closestPointWorld);
		newContact->penetration = sphereRadius - sqrtf(distanceSquared); 
		newContact->restitution = objectRestitution;
		newContact->friction = friction;
		newContact->normalImpulseSum = 0.0f;
		newContact->tangentImpulseSum1 = 0.0f;
		newContact->tangentImpulseSum2 = 0.0f;

		contacts.push_back(newContact);
		return true;
	}
	else
		return false;
}

bool CollisionDetector::CheckBoxBoxCollision(std::vector<Contact*>& contacts, BoxCollider* box1, BoxCollider* box2)
{
	Transform* box1Tr = box1->GetGameObject()->GetTransform();
	Transform* box2Tr = box2->GetGameObject()->GetTransform();

	RigidBody* box1Rb = box1->GetGameObject()->GetComponent<RigidBody>();
	RigidBody* box2Rb = box2->GetGameObject()->GetComponent<RigidBody>();

	/* ��ħ �˻��� ������ �� �� ���� */
	vector<Vector3> axes;

	/* box1 �� �� �� ���� */
	for (int i = 0; i < 3; ++i)
		axes.push_back(box1Tr->GetAxis(i));

	/* box2 �� �� �� ���� */
	for (int i = 0; i < 3; ++i)
		axes.push_back(box2Tr->GetAxis(i));

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
	Vector3 centerToCenter = box2Tr->GetPosition() - box1Tr->GetPosition();
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

	contacts.push_back(newContact);
	return true;
}




float CollisionDetector::calcPenetration(BoxCollider* box1, BoxCollider* box2, const Vector3& axis)
{
	Transform* box1Tr = box1->GetGameObject()->GetTransform(); 
	Transform* box2Tr = box2->GetGameObject()->GetTransform();

	Vec3 box1HalfSize = box1->GetSize() * box1Tr->GetScale() / 2;
	Vec3 box2HalfSize = box2->GetSize() * box2Tr->GetScale() / 2;

	/* �� �ڽ��� �߽� �� �Ÿ��� ����Ѵ� */
	Vector3 centerToCenter = box2Tr->GetPosition() - box1Tr->GetPosition();
	float projectedCenterToCenter = abs(centerToCenter.Dot(axis));

	/* �� �ڽ��� �־��� �࿡ �翵��Ų ������ ���� ����Ѵ� */
	float projectedSum =
		  abs((box1Tr->GetAxis(0) * box1HalfSize.x).Dot(axis)) 
		+ abs((box1Tr->GetAxis(1) * box1HalfSize.y).Dot(axis)) 
		+ abs((box1Tr->GetAxis(2) * box1HalfSize.z).Dot(axis))
		+ abs((box2Tr->GetAxis(0) * box2HalfSize.x).Dot(axis))
		+ abs((box2Tr->GetAxis(1) * box2HalfSize.y).Dot(axis))
		+ abs((box2Tr->GetAxis(2) * box2HalfSize.z).Dot(axis));

	/* "�翵��Ų ������ �� - �߽� �� �Ÿ�" �� ��ģ �����̴� */
	return projectedSum - projectedCenterToCenter;
}



// Fixed
// ��-�� ������ ��
void CollisionDetector::calcContactPointOnPlane(
	BoxCollider* box1,
	BoxCollider* box2,
	int minAxisIdx,
	Contact* contact
)
{
	Transform* box1Tr = box1->GetGameObject()->GetTransform(); 
	Transform* box2Tr = box2->GetGameObject()->GetTransform(); 

	Vec3 box1HalfSize = box1->GetSize() * 0.5f; 
	Vec3 box2HalfSize = box2->GetSize() * 0.5f;

	/* �浹 ���� */
	Vec3* contactPoint1;
	Vec3* contactPoint2;

	if (minAxisIdx < 3) // �浹���� box1 �� ���� ��
	{
		contactPoint2 = new Vec3(box2HalfSize.x, box2HalfSize.y, box2HalfSize.z);

		if (box2Tr->GetAxis(0).Dot(contact->normal) < 0)
			contactPoint2->x *= -1.0f;
		if (box2Tr->GetAxis(1).Dot(contact->normal) < 0)
			contactPoint2->y *= -1.0f;
		if (box2Tr->GetAxis(2).Dot(contact->normal) < 0)
			contactPoint2->z *= -1.0f;

		/* ���� ��ǥ�� ��ȯ�Ѵ� */
		*contactPoint2 = Vec3::Transform(*contactPoint2, box2Tr->GetWorldMatrix());
		contactPoint1 = new Vec3(*contactPoint2 - contact->normal * contact->penetration);
	}
	else // �浹���� box2 �� ���� ��
	{
		contactPoint1 = new Vec3(box1HalfSize.x, box1HalfSize.y, box1HalfSize.z);

		if (box1Tr->GetAxis(0).Dot(contact->normal) > 0)
			contactPoint1->x *= -1.0f;
		if (box1Tr->GetAxis(1).Dot(contact->normal) > 0)
			contactPoint1->y *= -1.0f;
		if (box1Tr->GetAxis(2).Dot(contact->normal) > 0)
			contactPoint1->z *= -1.0f;

		/* ���� ��ǥ�� ��ȯ�Ѵ� */
		*contactPoint1 = Vec3::Transform(*contactPoint1, box1Tr->GetWorldMatrix()); 
		contactPoint2 = new Vec3(*contactPoint1 - contact->normal * contact->penetration);
	}

	contact->contactPoint[0] = *contactPoint1;
	contact->contactPoint[1] = *contactPoint2;
}




void CollisionDetector::calcContactPointOnLine(
	BoxCollider* box1,
	BoxCollider* box2,
	int minAxisIdx,
	Contact* contact
)
{
	Transform* box1Tr = box1->GetGameObject()->GetTransform(); 
	Transform* box2Tr = box2->GetGameObject()->GetTransform(); 

	Vec3 box1HalfSize = box1->GetSize() * 0.5f; 
	Vec3 box2HalfSize = box2->GetSize() * 0.5f; 

	/* ������ �� ���� ������ ã�´� */
	Vec3 vertexOne(box1HalfSize.x, box1HalfSize.y, box1HalfSize.z);
	Vec3 vertexTwo(box2HalfSize.x, box2HalfSize.y, box2HalfSize.z);

	if (box1Tr->GetAxis(0).Dot(contact->normal) > 0)
		vertexOne.x *= -1.0f;
	if (box1Tr->GetAxis(1).Dot(contact->normal) > 0)
		vertexOne.y *= -1.0f;
	if (box1Tr->GetAxis(2).Dot(contact->normal) > 0)
		vertexOne.z *= -1.0f;

	if (box2Tr->GetAxis(0).Dot(contact->normal) < 0)
		vertexTwo.x *= -1.0f;
	if (box2Tr->GetAxis(1).Dot(contact->normal) < 0)
		vertexTwo.y *= -1.0f;
	if (box2Tr->GetAxis(2).Dot(contact->normal) < 0)
		vertexTwo.z *= -1.0f;

	/* ���� ������ ã�´� */
	Vector3 directionOne, directionTwo;

	switch (minAxisIdx)
	{
	case 6: // box1 �� x �� X box2 �� x ��
		directionOne = box1Tr->GetAxis(0);
		if (vertexOne.x > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(0);
		if (vertexTwo.x > 0) directionTwo *= -1.0f;
		break;

	case 7: // box1 �� x �� X box2 �� y ��
		directionOne = box1Tr->GetAxis(0);
		if (vertexOne.x > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(1);
		if (vertexTwo.y > 0) directionTwo *= -1.0f;
		break;

	case 8: // box1 �� x �� X box2 �� z ��
		directionOne = box1Tr->GetAxis(0);
		if (vertexOne.x > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(2);
		if (vertexTwo.z > 0) directionTwo *= -1.0f;
		break;

	case 9: // box1 �� y �� X box2 �� x ��
		directionOne = box1Tr->GetAxis(1);
		if (vertexOne.y > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(0);
		if (vertexTwo.x > 0) directionTwo *= -1.0f;
		break;

	case 10: // box1 �� y �� X box2 �� y ��
		directionOne = box1Tr->GetAxis(1);
		if (vertexOne.y > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(1);
		if (vertexTwo.y > 0) directionTwo *= -1.0f;
		break;

	case 11: // box1 �� y �� X box2 �� z ��
		directionOne = box1Tr->GetAxis(1);
		if (vertexOne.y > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(2);
		if (vertexTwo.z > 0) directionTwo *= -1.0f;
		break;

	case 12: // box1 �� z �� X box2 �� x ��
		directionOne = box1Tr->GetAxis(2);
		if (vertexOne.z > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(0);
		if (vertexTwo.x > 0) directionTwo *= -1.0f;
		break;

	case 13: // box1 �� z �� X box2 �� y ��
		directionOne = box1Tr->GetAxis(2);
		if (vertexOne.z > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(1);
		if (vertexTwo.y > 0) directionTwo *= -1.0f;
		break;

	case 14: // box1 �� z �� X box2 �� z ��
		directionOne = box1Tr->GetAxis(2);
		if (vertexOne.z > 0) directionOne *= -1.0f;
		directionTwo = box2Tr->GetAxis(2);
		if (vertexTwo.z > 0) directionTwo *= -1.0f;
		break;

	default:
		break;
	}

	/* ������ ���� ��ǥ��� ��ȯ�Ѵ� */
	vertexOne = Vec3::Transform(vertexOne, box1Tr->GetWorldMatrix());
	vertexTwo = Vec3::Transform(vertexTwo, box2Tr->GetWorldMatrix());

	/* box2 �� ���� ���� ����� box1 ���� ���� ã�´� */
	float k = directionOne.Dot(directionTwo);
	Vector3* closestPointOne = new Vector3(vertexOne + directionOne * ((vertexTwo - vertexOne).Dot(directionOne - directionTwo * k) / (1 - k * k)));
	/* box1 �� ���� ���� ����� box2 ���� ���� ã�´� */
	Vector3* closestPointTwo = new Vector3(vertexTwo + directionTwo * ((*closestPointOne - vertexTwo).Dot(directionTwo)));

	contact->contactPoint[0] = *closestPointOne;
	contact->contactPoint[1] = *closestPointTwo;
}