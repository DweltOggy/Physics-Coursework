#pragma once
#include "..\CSC8503Common\GameObject.h"
namespace NCL
{
	namespace CSC8503
	{
		class StateMachine;
		class MovingPlatform :public GameObject
		{

		public:
			MovingPlatform();
			~MovingPlatform();

			void Update(float dt);
			void OnCollisionBegin(GameObject* otherObject) override;

		protected:
			void MoveLeft(float dt);
			void MoveRight(float dt);

			void MoveForward(float dt);
			void MoveBackward(float dt);

			int direction;

			StateMachine* stateMachine;
		};

	}
}