#include "BonusGameObject.h"
#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;

void BonusGameObject::OnCollisionBegin(GameObject* otherObject) 
{
	if (otherObject->GetType() == Players && isActive)
	{
		isActive = false;
		//TEMPORARY
		delete boundingVolume;
		SetBoundingVolume(nullptr);
	}
}

void BonusGameObject::Update(float dt)
{
	float turnSpeed = 100.0f * dt;
	//a bit of spin for fun
	physicsObject->AddTorque(Vector3(0, 1, 0) * turnSpeed);

}
