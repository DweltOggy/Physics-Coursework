#include "MovingGameObject.h"
#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;

void MovingGameObject::Update(float dt)
{
	time += dt;
	if (time > 1.0)
	{
		physicsObject->AddForce(Vector3(1, 0, 0) * 2000.0f);
		time = 0;
	}

	physicsObject->AddForce(Vector3(0, 1, 0) * 100.0f);
}