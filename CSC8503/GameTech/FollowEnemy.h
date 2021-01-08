#pragma once
#include "..\CSC8503Common\GameObject.h"

namespace NCL
{
	namespace CSC8503
	{
		class StateMachine;
		class FollowEnemy : public GameObject
		{
		public:
			FollowEnemy();
			~FollowEnemy();

			void setTarget(GameObject* a)
			{
				target = a;
				relativePos =
					target->GetTransform().GetPosition() - GetTransform().GetPosition();

				currentDistance = relativePos.Length();

			}
			void Update(float dt);
			void OnCollisionBegin(GameObject* otherObject) override;
			void followTarget(float dt);

		protected:

			StateMachine* stateMachine;
			GameObject* target;

			Vector3 relativePos;

			float distance;
			float currentDistance;
		};
	}
}