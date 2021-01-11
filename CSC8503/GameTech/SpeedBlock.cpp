#include "SpeedBlock.h"

#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;

void SpeedBlock::OnCollisionBegin(GameObject* otherObject)
{

}

void SpeedBlock:: OnCollisionEnd(GameObject* otherObject)
{	
	Vector3 force = GetTransform().GetOrientation() * Vector3(0, 0, -5000);
	otherObject->GetPhysicsObject()->AddForce(force);
}