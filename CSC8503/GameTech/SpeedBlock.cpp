#include "SpeedBlock.h"

#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;

void SpeedBlock::OnCollisionBegin(GameObject* otherObject)
{
	//Vector3 force = GetTransform().GetOrientation() * Vector3(0, -10000, 0);
	if(otherObject->GetType() == Players)
		otherObject->GetPhysicsObject()->AddForce(Vector3(0, 10000, 0));
}

void SpeedBlock:: OnCollisionEnd(GameObject* otherObject)
{	

}