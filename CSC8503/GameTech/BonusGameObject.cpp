#include "BonusGameObject.h"
#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;

void BonusGameObject::OnCollisionBegin(GameObject* otherObject) 
{
	if (otherObject->GetType() == Players && isActive)
	{
		isActive = false;
		std::cout << "Bonus Collided!" << std::endl;

		//TEMPORARY
		delete boundingVolume;
		SetBoundingVolume(nullptr);
	}
}
