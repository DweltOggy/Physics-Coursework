#pragma once
#include "..\CSC8503Common\GameObject.h"
namespace NCL
{
	namespace CSC8503
	{
		class BonusGameObject : public GameObject
		{
			void OnCollisionBegin(GameObject* otherObject) override;

		};
	}
}


