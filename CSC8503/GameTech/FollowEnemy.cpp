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
	scoreTolereance = 100;

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

	State* harrasPlayer = new State([&](float dt) -> void
		{
			this->seekPlayer(dt);
		}
	);

	
	stateMachine->AddState(seekExit);
	stateMachine->AddState(seekBonus);
	stateMachine->AddState(harrasPlayer);

	stateMachine->AddTransition(new StateTransition(seekExit, seekBonus,
		[&]() -> bool
		{
	
			if (this->getScore() < scoreTolereance)
			{
				nearestBonus = worldRef->closestBonus(this);
				
				if (nearestBonus == nullptr)
					return false;

				if((exit->GetTransform().GetPosition() - GetTransform().GetPosition()).Length() <
					(nearestBonus->GetTransform().GetPosition() - GetTransform().GetPosition()).Length())
				{
					return false;
				}	

				return true;
			}
			return false;
		}
	));

	stateMachine->AddTransition(new StateTransition(seekBonus, seekExit,
		[&]() -> bool
		{
			return !nearestBonus->IsActive();
		}
	));


	stateMachine->AddTransition(new StateTransition(seekExit, harrasPlayer,
		[&]() -> bool
		{
			if (player->getScore() > score &&
				(player->GetTransform().GetPosition() - GetTransform().GetPosition()).Length() < 5.0f )
				return true;
			
			return false;
		}
	));

	stateMachine->AddTransition(new StateTransition(harrasPlayer, seekExit,
		[&]() -> bool
		{
			if ((player->GetTransform().GetPosition() - GetTransform().GetPosition()).Length() > 5.0f)
				return true;

			return false;
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
	relativePos = exit->GetTransform().GetPosition() - GetTransform().GetPosition();

	Vector3 offsetDir = relativePos.Normalised();

	GetPhysicsObject()->AddForce(offsetDir * 100.0f);
}

void FollowEnemy::seekBonus(float dt)
{
	relativePos = nearestBonus->GetTransform().GetPosition() - GetTransform().GetPosition();

	Vector3 offsetDir = relativePos.Normalised();

	GetPhysicsObject()->AddForce(offsetDir * 100.0f);
}


void FollowEnemy::seekPlayer(float dt)
{
	relativePos = player->GetTransform().GetPosition() - GetTransform().GetPosition();

	Vector3 offsetDir = relativePos.Normalised();

	GetPhysicsObject()->AddForce(offsetDir * 100.0f);
}