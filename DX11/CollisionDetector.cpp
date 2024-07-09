#include "pch.h"
#include "CollisionDetector.h"

CollisionDetector::CollisionDetector()
	: friction(0.6f),
	objectRestitution(0.3f),
	groundRestitution(0.2f) 
{
}

void CollisionDetector::DetectCollision(std::vector<Contact*>& contacts, vector<GameObject*>& gameObjects)
{
    for (size_t i = 0; i < gameObjects.size(); ++i)
    {
        Collider* colliderPtrI = gameObjects[i]->GetComponent<Collider>();
        if (!colliderPtrI) continue;

        for (size_t j = i + 1; j < gameObjects.size(); ++j)
        {
            Collider* colliderPtrJ = gameObjects[j]->GetComponent<Collider>(); 
            if (!colliderPtrJ) continue;

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
    }
}


bool CollisionDetector::CheckSphereSphereCollision(std::vector<Contact*>& contacts, SphereCollider* sphere1, SphereCollider* sphere2)
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

		contacts.push_back(newContact);
		return true;
	}
	else
		return false;
}

bool CollisionDetector::CheckSphereBoxCollision(std::vector<Contact*>& contacts, SphereCollider* sphere, BoxCollider* box)
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
	std::vector<Vector3> axes;

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

	contacts.push_back(newContact);
	return true;
}




float CollisionDetector::calcPenetration(BoxCollider* box1, BoxCollider* box2, const Vector3& axis)
{
	Transform* box1Tr = box1->GetGameObject()->GetTransform(); 
	Transform* box2Tr = box2->GetGameObject()->GetTransform();

	Vec3 box1HalfSize = box1->GetSize() / 2;
	Vec3 box2HalfSize = box1->GetSize() / 2;

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




void CollisionDetector::calcContactPointOnPlane(
	BoxCollider* box1,
	BoxCollider* box2,
	int minAxisIdx,
	Contact* contact
)
{
	Transform* box1Tr = box1->GetGameObject()->GetTransform(); 
	Transform* box2Tr = box2->GetGameObject()->GetTransform(); 

	Vec3 box1HalfSize = box1->GetSize() / 2;
	Vec3 box2HalfSize = box1->GetSize() / 2;

	/* �浹 ���� */
	Vec3 contactPoint1;
	Vec3 contactPoint2;

	if (minAxisIdx < 3) // �浹���� box1 �� ���� ��
	{
		contactPoint2 = Vec3(box2HalfSize.x, box2HalfSize.y, box2HalfSize.z);

		if (box2Tr->GetAxis(0).Dot(contact->normal) < 0)
			contactPoint2.x *= -1.0f;
		if (box2Tr->GetAxis(1).Dot(contact->normal) < 0)
			contactPoint2.y *= -1.0f;
		if (box2Tr->GetAxis(2).Dot(contact->normal) < 0)
			contactPoint2.z *= -1.0f;

		/* ���� ��ǥ�� ��ȯ�Ѵ� */
		contactPoint2 = Vec3::Transform(contactPoint2, box2Tr->GetWorldMatrix());
		contactPoint1 = Vec3(contactPoint2 - contact->normal * contact->penetration);
	}
	else // �浹���� box2 �� ���� ��
	{
		contactPoint1 = Vec3(box1HalfSize.x, box1HalfSize.y, box1HalfSize.z);

		if (box1Tr->GetAxis(0).Dot(contact->normal) > 0)
			contactPoint1.x *= -1.0f;
		if (box1Tr->GetAxis(1).Dot(contact->normal) > 0)
			contactPoint1.y *= -1.0f;
		if (box1Tr->GetAxis(2).Dot(contact->normal) > 0)
			contactPoint1.z *= -1.0f;

		/* ���� ��ǥ�� ��ȯ�Ѵ� */
		contactPoint1 = Vec3::Transform(contactPoint1, box1Tr->GetWorldMatrix());
		contactPoint2 = Vec3(contactPoint1 - contact->normal * contact->penetration);
	}

	contact->contactPoint[0] = contactPoint1;
	contact->contactPoint[1] = contactPoint2;
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

	Vec3 box1HalfSize = box1->GetSize() / 2;
	Vec3 box2HalfSize = box1->GetSize() / 2;

	/* ������ �� ���� ������ ã�´� */
	Vector3 vertexOne(box1HalfSize.x, box1HalfSize.y, box1HalfSize.z);
	Vector3 vertexTwo(box2HalfSize.x, box2HalfSize.y, box2HalfSize.z);

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
	Vec3 closestPointOne = Vec3(vertexOne + directionOne * ((vertexTwo - vertexOne).Dot(directionOne - directionTwo * k) / (1 - k * k)));

	/* box1 �� ���� ���� ����� box2 ���� ���� ã�´� */
	Vec3 closestPointTwo = Vec3(vertexTwo + directionTwo * ((closestPointOne - vertexTwo).Dot(directionTwo)));

	contact->contactPoint[0] = closestPointOne; 
	contact->contactPoint[1] = closestPointTwo; 
}