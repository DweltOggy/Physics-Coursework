#include "FollowEnemy.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/State.h"

using namespace NCL;
using namespace CSC8503;

FollowEnemy::FollowEnemy(GameWorld* a)
{
	nearestBonus = nullptr;
	exit = nullptr;
	worldRef = a;
	score = 600;
	currentDistance = 0.0;
	distance = 10.0f;

	stateMachine = new StateMachine();

	State* seekExit = new State([&](float dt) -> void
		{
			this->seekExit(dt);
		}
	);
	State* seekBonus = new State([&](float dt) -> void
		{
			this->seekBonus(dt);
		}
	);

	State* seekPlayer = new State([&](float dt) -> void
		{

		}
	);

	
	stateMachine->AddState(seekExit);
	stateMachine->AddState(seekBonus);

	stateMachine->AddTransition(new StateTransition(seekExit, seekBonus,
		[&]() -> bool
		{
			bool change = false;

			if (this->getScore() < 500)
			{
				change = true;
				nearestBonus = worldRef->closestBonus(this);

				if (nearestBonus == nullptr)
					change = false;
			}
			
			return change;
		}
	));

	stateMachine->AddTransition(new StateTransition(seekBonus, seekExit,
		[&]() -> bool
		{
			bool change = false;
			change = !nearestBonus->IsActive();
			return change;
		}
	));

}

FollowEnemy::~FollowEnemy()
{
	delete stateMachine;
}

void FollowEnemy::Update(float dt)
{
	if (isActive)
	{
		stateMachine->Update(dt);

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

void FollowEnemy::seekExit(float dt)
{
	relativePos =
		exit->GetTransform().GetPosition() - GetTransform().GetPosition();

	Vector3 offsetDir = relativePos.Normalised();

	GetPhysicsObject()->AddForce(offsetDir * 100.0f );
}

void FollowEnemy::seekBonus(float dt)
{
	relativePos =
		nearestBonus->GetTransform().GetPosition() - GetTransform().GetPosition();

	Vector3 offsetDir = relativePos.Normalised();

	GetPhysicsObject()->AddForce(offsetDir * 100.0f);
}
