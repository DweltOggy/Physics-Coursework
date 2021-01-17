#pragma once
#include "Constraint.h"

namespace NCL 
{
	namespace CSC8503 
	{
		class GameObject;
		class MotorConstraint : public Constraint
		{
		public:
			MotorConstraint(GameObject* a, GameObject* b, float d)
			{
				objectA = a;
			}
			~MotorConstraint() {}

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;

			float distance;
		};
	}
}
