#include "MotorConstraint.h"
#include "PhysicsSystem.h"

using namespace NCL;
using namespace CSC8503;

void MotorConstraint::UpdateConstraint(float dt)
{
	Vector3 Pos = objectA->GetTransform().GetPosition();

	PhysicsObject* physA = objectA->GetPhysicsObject();


}