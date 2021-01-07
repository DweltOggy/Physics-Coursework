# pragma once
#include "Constraint.h"

namespace NCL 
{
	namespace CSC8503 
	{

		class GameObject;
		class SwingConstraint : public Constraint
		{
			SwingConstraint(GameObject* a, GameObject* b)
			{
				objectA = a;
				objectB = b;
			}

			~SwingConstraint(){}

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;
			GameObject* objectB;

		};
	}
}
