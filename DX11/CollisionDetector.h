#pragma once

class CollisionDetector
{
private:
    float friction;
    float objectRestitution;
    float groundRestitution;

public:
    CollisionDetector();
      
public:
    /* �浹�� �����ϰ� �浹 ������ contacts �� �����Ѵ� */
    void DetectCollision(std::vector<Contact*>& contacts, vector<GameObject*>& gameObjects);
    void DetectCollision(std::vector<Contact*>& contacts, std::unordered_map<unsigned int, Collider*>& colliders);


private:
    /* �浹 �˻� �Լ���.
        �浹�� �ִٸ� �浹 ���� ����ü�� �����ϰ� contacts �� Ǫ���ϰ� true �� ��ȯ�Ѵ�.
        �ѵ��� ���ٸ� false �� ��ȯ�Ѵ� */
    bool CheckSphereSphereCollision(std::vector<Contact*>& contacts, SphereCollider* sphere1, SphereCollider* sphere2);
    bool CheckSphereBoxCollision(std::vector<Contact*>& contacts, SphereCollider* sphere, BoxCollider* box);
    bool CheckBoxBoxCollision(std::vector<Contact*>& contacts, BoxCollider* box1, BoxCollider* box2);

    /* ���� ������ ����ϴ��� �˻��Ѵ�
        ī�޶�κ��� hit point ������ �Ÿ��� ��ȯ�Ѵ�
        hit ���� �ʴ´ٸ� ������ ��ȯ�Ѵ� */
    float rayAndSphere(
        const Vector3& origin,
        const Vector3& direction,
        const SphereCollider&
    );

    float rayAndBox(
        const Vector3& origin,
        const Vector3& direction,
        const BoxCollider&
    );

private:
    /* �� �ڽ��� �־��� �࿡ ���� ������� ��ġ���� ��ȯ�Ѵ� */
    float calcPenetration(
        BoxCollider* box1,
        BoxCollider* box2,
        const Vector3& axis
    );

    /* ������ü�� ��-�� ������ �� �浹���� ã�´� */
    void calcContactPointOnPlane(
        BoxCollider*box1,
        BoxCollider*box2,
        int minPenetrationAxisIdx,
        Contact* contact
    );

    /* ������ü�� ��-�� ������ �� �浹���� ã�´� */
    void calcContactPointOnLine(
        BoxCollider* box1,
        BoxCollider* box2,
        int minPenetrationAxisIdx,
        Contact* contact
    );
};

