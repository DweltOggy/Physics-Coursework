#include "FollowEnemy.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/State.h"

using namespace NCL;
using namespace CSC8503;

FollowEnemy::FollowEnemy()
{
	target = nullptr;
	currentDistance = 0.0;
	distance = 10.0f;

	stateMachine = new StateMachine();

	State* following = new State([&](float dt) -> void
		{
			this->followTarget(dt);
		}
	);
	State* waiting = new State([&](float dt) -> void
		{

		}
	);


	stateMachine->AddState(following);
	stateMachine->AddState(waiting);

	stateMachine->AddTransition(new StateTransition(following, waiting,
		[&]() -> bool
		{
			return this->currentDistance < distance;
		}
	));

	stateMachine->AddTransition(new StateTransition(waiting, following,
		[&]() -> bool
		{
			return this->currentDistance > distance;
		}
	));

}

FollowEnemy::~FollowEnemy()
{
	delete stateMachine;
}

void FollowEnemy::Update(float dt)
{
	relativePos =
		target->GetTransform().GetPosition() - GetTransform().GetPosition();
	currentDistance = relativePos.Length();

	stateMachine->Update(dt);
}
void FollowEnemy::followTarget(float dt)
{
	relativePos =
		target->GetTransform().GetPosition() - GetTransform().GetPosition();

	Vector3 offsetDir = relativePos.Normalised();

	GetPhysicsObject()->AddForce(offsetDir * 100.0f );
}

void FollowEnemy::OnCollisionBegin(GameObject* otherObject)
{

}