#include "PlayerGameObject.h"
#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;

PlayerGameObject::PlayerGameObject(string name)
{
	time = 0;
	score = 1000;
	won = false;
	lost = false;
}

void PlayerGameObject::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetType() == bonus)
	{
		score += 25;
	}

	if (otherObject->GetType() == finish)
	{
		won = true;
		isActive = false;
	}
}

void PlayerGameObject::Update(float dt)
{
	if (isActive)
	{
		time += dt;
		if (time > 1.0 && won == false)
		{
			removeScore(10);
			time = 0;
		}
		if (getScore() <= 0)
		{
			isActive = false;
			lost = true;
		}
	}

}