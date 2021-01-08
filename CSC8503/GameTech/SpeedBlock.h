#pragma once
#include "..\CSC8503Common\GameObject.h"

namespace NCL
{
	namespace CSC8503
	{
		class SpeedBlock : public GameObject
		{
			void OnCollisionBegin(GameObject* otherObject) override;
			void OnCollisionEnd(GameObject* otherObject) override;
		};
	}
}