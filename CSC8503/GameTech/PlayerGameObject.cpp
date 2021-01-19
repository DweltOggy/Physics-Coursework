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
		Vector3 fwdAxis = GetTransform().GetOrientation() * Vector3(0, 0, 1);

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
			GetPhysicsObject()->AddForce(-fwdAxis * 100.0f);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
			GetPhysicsObject()->AddForce(fwdAxis * 100.0f);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
			GetPhysicsObject()->AddForce(Vector3(0, 6000.0f, 0));// *jumpForce);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
		{
			GetPhysicsObject()->AddTorque(-Vector3(0, 1, 0) * 30.0f);
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
		{
			GetPhysicsObject()->AddTorque(Vector3(0, 1, 0) * 30.0f);
		}
	}

}