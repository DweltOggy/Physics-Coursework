#pragma once
#include "..\CSC8503Common\GameObject.h"

namespace NCL
{
	namespace CSC8503
	{
		class PlayerGameObject : public GameObject
		{
		protected:
			void OnCollisionBegin(GameObject* otherObject) override;
			void Update(float dt) override;

		public:
			int score;
			float time = 0;
			bool won;
			bool lost;

			int getScore()
			{
				return score;
			}
			void setScore(int a)
			{
				score = a;
			}
			void addScore(int a)
			{
				score += a;
			}
			void removeScore(int a)
			{
				score -= a;
			}
			
			PlayerGameObject(string name = "");
		};

	}
}