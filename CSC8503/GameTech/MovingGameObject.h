#pragma once
#include "..\CSC8503Common\GameObject.h"
namespace NCL
{
	namespace CSC8503
	{
		class MovingGameObject : public GameObject
		{
		public:
			float time = 0;
			void Update(float dt) override;
		};

	}
}