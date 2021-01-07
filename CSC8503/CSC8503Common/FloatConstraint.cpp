#include "FloatConstraint.h"
using namespace NCL;
using namespace CSC8503;

void FloatConstraint::UpdateConstraint(float dt)
{
	yLevel.x = objectA->GetTransform().GetPosition().x;
	yLevel.z = objectA->GetTransform().GetPosition().z;

	Vector3 relativePos =
		objectA->GetTransform().GetPosition() - yLevel;

	float currentDistance = relativePos.Length();
	//float currentDistance = objectA->GetTransform().GetPosition().y

	if (abs(currentDistance) > 0.0f)
	{
		Vector3 offsetDir = relativePos.Normalised();

		PhysicsObject* physA = objectA->GetPhysicsObject();
		Vector3 relativeVelocity = physA->GetLinearVelocity();

		float constraintMass = physA->GetInverseMass();
		if (constraintMass > 0.0f)
		{
			// how much of their relative force is affecting the constraint
			float velocityDot = Vector3::Dot(relativeVelocity, offsetDir);

			float biasFactor = 0.01f;
			float bias = -(biasFactor / dt) * -currentDistance;

			float lambda = -(velocityDot + bias) / constraintMass;

			Vector3 Impulse = offsetDir * lambda;

			physA->ApplyLinearImpulse(Impulse); // multiplied by mass here
		}
	}
}