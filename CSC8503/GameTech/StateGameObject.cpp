#include "StateGameObject.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/State.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject() 
{
	counter = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0));
	stateMachine = new StateMachine();
	
	State * stateA = new State([&](float dt) -> void
		{
			this -> MoveLeft(dt);
		}
	);
	State * stateB = new State([&](float dt) -> void
	{
		this -> MoveRight(dt);
	}
	);
	
	
	stateMachine -> AddState(stateB);
	stateMachine -> AddState(stateA);
	
	stateMachine -> AddTransition(new StateTransition(stateA, stateB,
	[&]() -> bool
		{
			return this -> counter > 1.0f;
		}
	));
	
	stateMachine -> AddTransition(new StateTransition(stateB, stateA,
	[&]() -> bool
		{
			return this -> counter < 0.0f;
		}
	));
	
}

StateGameObject ::~StateGameObject() 
{
	delete stateMachine;
}

void StateGameObject::Update(float dt) 
{
	stateMachine -> Update(dt);
}

void StateGameObject::MoveLeft(float dt) 
{
	GetPhysicsObject() -> AddForce({ -400 , 0 , 0 });
	counter += dt;
}

void StateGameObject::MoveRight(float dt) 
{
	GetPhysicsObject() -> AddForce({ 400 , 0 , 0 });
	counter -= dt;
}
