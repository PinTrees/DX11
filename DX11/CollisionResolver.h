#pragma once

class CollisionResolver
{
private:
	float penetrationTolerance = 0.0005f;
	float closingSpeedTolerance = 0.005f;

public:
	void ResolveCollision(std::vector<Contact*>& contacts, float deltaTime);

private:
	void sequentialImpulse(Contact* contact, float deltaTime);
};

