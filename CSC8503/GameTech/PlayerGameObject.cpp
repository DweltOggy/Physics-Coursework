#include "PlayerGameObject.h"
#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;

PlayerGameObject::PlayerGameObject(string name)
{
	score = 1000;
	//GameObject::GameObject(name);
}

void PlayerGameObject::OnCollisionBegin(GameObject* otherObject)
{
	if (otherObject->GetType() == bonus)
	{
		score += 25;
	}
}
