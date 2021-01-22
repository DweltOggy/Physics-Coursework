#include "MovingPlatform.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/State.h"

#include "TutorialGame.h"
using namespace NCL;
using namespace CSC8503;


MovingPlatform::MovingPlatform()
{
	stateMachine = new StateMachine();
	direction = 0;

	State* forward = new State([&](float dt)-> void 
		{
			this->MoveForward(dt);
		});
	State* backward = new State([&](float dt)-> void
		{
			this->MoveBackward(dt);
		});
	State* left = new State([&](float dt)-> void
		{
			this->MoveLeft(dt);
		});
	State* right = new State([&](float dt)-> void
		{
			this->MoveRight(dt);
		});

	stateMachine->AddState(forward);
	stateMachine->AddState(backward);
	stateMachine->AddState(left);
	stateMachine->AddState(right);

	stateMachine->AddTransition(new StateTransition(forward, left,
		[&]() -> bool
		{
			return GetTransform().GetPosition().z < -780;
		}
	));

	stateMachine->AddTransition(new StateTransition(left, backward,
		[&]() -> bool
		{
			return GetTransform().GetPosition().x < -55;
		}
	));

	stateMachine->AddTransition(new StateTransition(backward, right,
		[&]() -> bool
		{
			return GetTransform().GetPosition().z > -620;
		}
	));

	stateMachine->AddTransition(new StateTransition(right, forward,
		[&]() -> bool
		{
			return GetTransform().GetPosition().x > 55;
		}
	));
}

MovingPlatform::~MovingPlatform()
{
	delete stateMachine;
}

void MovingPlatform::Update(float dt)
{
	//GetPhysicsObject()->AddForce({ 0 , 1000 , 0 });
	stateMachine->Update(dt);
}

void MovingPlatform::OnCollisionBegin(GameObject* otherObject)
{
	
}

void MovingPlatform::MoveForward(float dt)
{
	GetPhysicsObject()->AddForce({ 0 , 0 , -300 });
}

void MovingPlatform::MoveBackward(float dt)
{
	GetPhysicsObject()->AddForce({ 0 , 0 , 300 });
}

void MovingPlatform::MoveLeft(float dt)
{
	GetPhysicsObject()->AddForce({ -300 , 0 , 0 });
}

void MovingPlatform::MoveRight(float dt)
{
	GetPhysicsObject()->AddForce({300 , 0 , 0 });
}