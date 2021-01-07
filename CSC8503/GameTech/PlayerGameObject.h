#pragma once
#include "..\CSC8503Common\GameObject.h"
namespace NCL
{
	namespace CSC8503
	{
		class PlayerGameObject : public GameObject
		{
			
			void OnCollisionBegin(GameObject* otherObject) override;

		public:
			int score;
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