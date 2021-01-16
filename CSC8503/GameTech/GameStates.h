#pragma once
#include "../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/PushdownState.h"
#include "../../Common/Window.h"

#include "TutorialGame.h"

 namespace NCL
 {
	namespace CSC8503
	{
		class menuState : public PushdownState
		{
		public:
			menuState(TutorialGame* a)
			{
				g = a;
			}

			PushdownResult OnUpdate(float dt, PushdownState** newState) override;

			void OnAwake() override;

		protected:
			TutorialGame* g;
		};

		class singlePlayerState : public PushdownState
		{
		public:
			singlePlayerState(TutorialGame* a)
			{
				g = a;
			}

			PushdownResult OnUpdate(float dt, PushdownState** newState) override;
			void OnAwake() override;

		protected:
			TutorialGame* g;
		};

		class twoPlayerState : public PushdownState
		{
		public:
			twoPlayerState(TutorialGame* a)
			{
				g = a;
			}

			PushdownResult OnUpdate(float dt, PushdownState** newState) override;

			void OnAwake() override;

		protected:
			TutorialGame* g;
		};

		class TestState : public PushdownState
		{
		public:
			TestState(TutorialGame* a)
			{
				g = a;
			}
			PushdownResult OnUpdate(float dt, PushdownState** newState) override;

			void OnAwake() override;

		protected:
			TutorialGame* g;
		};

	}
 }
  












