#include "MotorConstraint.h"
#include "PhysicsSystem.h"

using namespace NCL;
using namespace CSC8503;

void MotorConstraint::UpdateConstraint(float dt)
{
	Vector3 relativePos = objectA->GetTransform().GetPosition() - objectB->GetTransform().GetPosition();
	Vector3 offsetDir = relativePos.Normalised();

	PhysicsObject* physA = objectA->GetPhysicsObject();
	PhysicsObject* physB = objectB->GetPhysicsObject();

	Vector3 relativeVelocity = physA->GetLinearVelocity() -
		physB->GetLinearVelocity();

	float constraintMass = physA->GetInverseMass() +
		physB->GetInverseMass();

	if (constraintMass > 0.0f)
	{

	}
}