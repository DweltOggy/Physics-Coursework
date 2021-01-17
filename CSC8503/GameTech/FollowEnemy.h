#pragma once
#include "..\CSC8503Common\GameObject.h"
#include "PlayerGameObject.h"
#include "..\CSC8503Common\GameWorld.h"

namespace NCL
{
	namespace CSC8503
	{
		class StateMachine;
		class FollowEnemy : public PlayerGameObject// public GameObject 
		{
		public:
			FollowEnemy(GameWorld* a);
			~FollowEnemy();

			void setTarget(GameObject* a)
			{
				exit = a;
				relativePos =
					exit->GetTransform().GetPosition() - GetTransform().GetPosition();

				currentDistance = relativePos.Length();

			}
			void Update(float dt) override;
			void seekExit(float dt);
			void seekBonus(float dt);

		protected:

			StateMachine* stateMachine;
			GameObject* nearestBonus;
			GameObject* exit;

			GameWorld* worldRef;

			Vector3 relativePos;

			float distance;
			float currentDistance;
			
		};
	}
}