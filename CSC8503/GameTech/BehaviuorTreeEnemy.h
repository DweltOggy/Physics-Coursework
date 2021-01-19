#pragma once
#include "..\CSC8503Common\GameObject.h"
#include "PlayerGameObject.h"
#include "..\CSC8503Common\GameWorld.h"
#include "../CSC8503Common/BehaviourAction.h"
#include "../CSC8503Common/BehaviourSequence.h"
#include "../CSC8503Common/BehaviourSelector.h"
#include "../CSC8503Common/BehaviourNodeWithChildren.h"
#include "../CSC8503Common/BehaviourNode.h"


namespace NCL
{
	namespace CSC8503
	{

		class BehaviuorTreeEnemy : public PlayerGameObject
		{
		public:
			BehaviuorTreeEnemy(GameWorld* a);
			~BehaviuorTreeEnemy();

			void setTarget(GameObject* a)
			{
				exit = a;
				relativePos =
					exit->GetTransform().GetPosition() - GetTransform().GetPosition();

			}

			void Update(float dt) override;

		protected:

			BehaviourSequence* rootSequence;
			BehaviourSequence* sequence;
			BehaviourSelector* selection;

			GameObject* nearestBonus;
			GameObject* exit;

			GameWorld* worldRef;
			Vector3 relativePos;

			BehaviourState state = Ongoing;

			int scoreTolereance;


		};

	}
}