#pragma once

class GameObject;

union COLLIDER_ID
{
	struct
	{
		UINT Left_id;
		UINT RIght_id;
	};

	ULONGLONG ID;
};

struct Contact
{
	Vec3 point;				// �浹 ����
	Vec3 normal;			// �浹 ���� ���� ����
	float penetrationDepth; // ħ�� ����
};


class PhysicsManager
{
	SINGLE_HEADER(PhysicsManager)

private:
	float m_GravityAcceleration;
	map<ULONGLONG, bool>	mMapColInfo;	// �浹ü ���� ���� ������ �浹 ���� 

public:
	void SetGravity(float gravity) { m_GravityAcceleration = gravity; }
	float GetGravity() const { return m_GravityAcceleration; }

public:
	void Init();
	void Update(float deltaTime);

private:
	void UpdateCollision();

	bool CheckSphereSphereCollision(SphereCollider* sphere1, SphereCollider* sphere2, Contact& contact);
	bool CheckSphereBoxCollision(SphereCollider* sphere, BoxCollider* box, Contact& contact);
	bool CheckBoxBoxCollision(BoxCollider* box1, BoxCollider* box2);
	void HandleCollision(GameObject* obj1, GameObject* obj2, COLLIDER_ID colliderID);

	void ResolveCollision(GameObject* obj1, GameObject* obj2, const Contact& contact);
};

