#include "pch.h"
#include "CollisionResolver.h"

void CollisionResolver::ResolveCollision(std::vector<Contact*>& contacts, float deltaTime)
{
    for (int i = 0; i < 30; ++i)
    {
        for (auto& contact : contacts)
        {
            sequentialImpulse(contact, deltaTime);
        }
    }
}

void CollisionResolver::sequentialImpulse(Contact* contact, float deltaTime)
{
    float effectiveMass;

    float totalInvMass = contact->bodies[0]->GetInverseMass();
    if (contact->bodies[1] != nullptr)
        totalInvMass += contact->bodies[1]->GetInverseMass();
    if (totalInvMass == 0.0f)
        return;

    Vector3 contactPointFromCenter1 = contact->contactPoint[0] - contact->bodies[0]->GetGameObject()->GetTransform()->GetPosition();
    Vector3 contactPointFromCenter2;
    if (contact->bodies[1] != nullptr)
        contactPointFromCenter2 = contact->contactPoint[1] - contact->bodies[1]->GetGameObject()->GetTransform()->GetPosition();

    Vector3 termInDenominator1 = (contact->bodies[0]->GetInverseInertiaTensorWorld() * (contactPointFromCenter1.Cross(contact->normal)))
        .Cross(contactPointFromCenter1);
    Vector3 termInDenominator2;
    if (contact->bodies[1] != nullptr) {
        termInDenominator2 =
            (contact->bodies[1]->GetInverseInertiaTensorWorld() * (contactPointFromCenter2.Cross(contact->normal)))
            .Cross(contactPointFromCenter2);
    }
    effectiveMass = totalInvMass + (termInDenominator1 + termInDenominator2).Dot(contact->normal);
    if (effectiveMass == 0.0f)
        return;

    float closingSpeed = contact->normal.Dot(contact->bodies[0]->GetVelocity())
        + contact->bodies[0]->GetRotationVelocity().Dot(contactPointFromCenter1.Cross(contact->normal));
    if (contact->bodies[1] != nullptr)
    {
        closingSpeed -= contact->normal.Dot(contact->bodies[1]->GetVelocity())
            + contact->bodies[1]->GetRotationVelocity().Dot(contactPointFromCenter2.Cross(contact->normal));
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
    contact->bodies[0]->SetRotationVelocity(
        contact->bodies[0]->GetRotationVelocity() + (contact->bodies[0]->GetInverseInertiaTensorWorld() * angularImpulse1)
    );
    if (contact->bodies[1] != nullptr)
    {
        contact->bodies[1]->SetVelocity(
            contact->bodies[1]->GetVelocity() - linearImpulse * contact->bodies[1]->GetInverseMass()
        );
        contact->bodies[1]->SetRotationVelocity(
            contact->bodies[1]->GetRotationVelocity() - (contact->bodies[1]->GetInverseInertiaTensorWorld() * angularImpulse2)
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
    termInDenominator1 = (contact->bodies[0]->GetInverseInertiaTensorWorld() * (contactPointFromCenter1.Cross(tangent1)))
        .Cross(contactPointFromCenter1);
    if (contact->bodies[1] != nullptr) {
        termInDenominator2 =
            (contact->bodies[1]->GetInverseInertiaTensorWorld() * (contactPointFromCenter2.Cross(tangent1)))
            .Cross(contactPointFromCenter2);
    }
    effectiveMass = totalInvMass + (termInDenominator1 + termInDenominator2).Dot(tangent1);

    closingSpeed = tangent1.Dot(contact->bodies[0]->GetVelocity())
        + contact->bodies[0]->GetRotationVelocity().Dot(contactPointFromCenter1.Cross(tangent1));
    if (contact->bodies[1] != nullptr)
    {
        closingSpeed -= tangent1.Dot(contact->bodies[1]->GetVelocity())
            + contact->bodies[1]->GetRotationVelocity().Dot(contactPointFromCenter2.Cross(tangent1));
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
    contact->bodies[0]->SetRotationVelocity(
        contact->bodies[0]->GetRotationVelocity() + contact->bodies[0]->GetInverseInertiaTensorWorld() * angularImpulse1
    );
    if (contact->bodies[1] != nullptr)
    {
        contact->bodies[1]->SetVelocity(
            contact->bodies[1]->GetVelocity() - linearImpulse * contact->bodies[1]->GetInverseMass()
        );
        contact->bodies[1]->SetRotationVelocity(
            contact->bodies[1]->GetRotationVelocity() - contact->bodies[1]->GetInverseInertiaTensorWorld() * angularImpulse2
        );
    }

    /* tangent2 ���Ϳ� ���� ���� ��� */
    termInDenominator1 = (contact->bodies[0]->GetInverseInertiaTensorWorld() * (contactPointFromCenter1.Cross(tangent2)))
        .Cross(contactPointFromCenter1);
    if (contact->bodies[1] != nullptr) {
        termInDenominator2 =
            (contact->bodies[1]->GetInverseInertiaTensorWorld() * (contactPointFromCenter2.Cross(tangent2)))
            .Cross(contactPointFromCenter2);
    }
    effectiveMass = totalInvMass + (termInDenominator1 + termInDenominator2).Dot(tangent2);

    closingSpeed = tangent2.Dot(contact->bodies[0]->GetVelocity())
        + contact->bodies[0]->GetRotationVelocity().Dot(contactPointFromCenter1.Cross(tangent2));
    if (contact->bodies[1] != nullptr)
    {
        closingSpeed -= tangent2.Dot(contact->bodies[1]->GetVelocity())
            + contact->bodies[1]->GetRotationVelocity().Dot(contactPointFromCenter2.Cross(tangent2));
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
    contact->bodies[0]->SetRotationVelocity(
        contact->bodies[0]->GetRotationVelocity() + (contact->bodies[0]->GetInverseInertiaTensorWorld() * angularImpulse1)
    );
    if (contact->bodies[1] != nullptr)
    {
        contact->bodies[1]->SetVelocity(
            contact->bodies[1]->GetVelocity() - linearImpulse * contact->bodies[1]->GetInverseMass()
        );
        contact->bodies[1]->SetRotationVelocity(
            contact->bodies[1]->GetRotationVelocity() - (contact->bodies[1]->GetInverseInertiaTensorWorld() * angularImpulse2)
        );
    }
}
