#include "PlayerGameObject.h"
#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;

PlayerGameObject::PlayerGameObject(string name)
{
	score = 1000;
}

void PlayerGameObject::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetType() == bonus)
	{
		score += 25;
	}
}

void PlayerGameObject::Update(float dt)
{
	if (isActive)
	{
		time += dt;
		if (time > 1.0 && isActive)
		{
			removeScore(10);
			time = 0;
		}
		if (getScore() <= 0)
		{
			isActive = false;
		}
	}

}