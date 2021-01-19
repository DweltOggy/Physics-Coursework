#include "BehaviuorTreeEnemy.h"


BehaviuorTreeEnemy::BehaviuorTreeEnemy(GameWorld* a)
{

	nearestBonus = nullptr;
	exit = nullptr;
	worldRef = a;
	score = 600;
	scoreTolereance = 550;
	nearestBonus = worldRef->closestBonus(this);

	BehaviourAction* seekBonus = new BehaviourAction
	("seeking Bonus", [&](float dt, BehaviourState state) -> BehaviourState
		{
			if (state == Initialise)
			{
				nearestBonus = worldRef->closestBonus(this);
				if (nearestBonus == nullptr || !nearestBonus->IsActive())
				{
					return Failure;
				}

				if ((exit->GetTransform().GetPosition() - GetTransform().GetPosition()).Length() <
					(nearestBonus->GetTransform().GetPosition() - GetTransform().GetPosition()).Length())
				{
					return Failure;
				}
				return Ongoing;

			}
			else if (state == Ongoing)
			{

				if (nearestBonus->IsActive())
				{
					relativePos = nearestBonus->GetTransform().GetPosition() - GetTransform().GetPosition();
					Vector3 offsetDir = relativePos.Normalised();
					GetPhysicsObject()->AddForce(offsetDir * 100.0f);
					return Ongoing;
				}
				return Success;
			}
			return state;
		}
	);

	BehaviourAction* Celebrate = new BehaviourAction
	(" quick Celebration", [&](float dt, BehaviourState state) -> BehaviourState
		{
			if (state == Initialise)
			{
				GetPhysicsObject()->AddForce(Vector3(0, 6000.0f, 0));
				return Success;
			}
			return state;
		}
	);

	BehaviourAction* seekExit = new BehaviourAction
	("seek exit", [&](float dt, BehaviourState state) -> BehaviourState
		{
			if (state == Initialise)
			{
				
				return Ongoing;
			}
			else if (state == Ongoing)
			{
				if(score > scoreTolereance)
				{
					relativePos = exit->GetTransform().GetPosition() - GetTransform().GetPosition();
					Vector3 offsetDir = relativePos.Normalised();
					GetPhysicsObject()->AddForce(offsetDir * 100.0f);
					return Ongoing;
				}
				else
				{
					return Failure;
				}
			}
			return state;
		}
	);

	//sequence = new BehaviourSequence(" bonus Sequence ");


	//sequence->AddChild(Celebrate);

	selection = new BehaviourSelector(" Loot Selection ");
	selection->AddChild(seekExit);
	selection->AddChild(seekBonus);

	rootSequence = new BehaviourSequence(" Root Sequence ");

	rootSequence->AddChild(selection);
	//rootSequence->AddChild(sequence);

	
}

BehaviuorTreeEnemy::~BehaviuorTreeEnemy()
{

}


void BehaviuorTreeEnemy::Update(float dt)
{
	if (isActive)
	{
		state = rootSequence->Execute(dt);

		if (state != Ongoing)
		{
			rootSequence->Reset();
		}
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