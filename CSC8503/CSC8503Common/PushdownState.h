#pragma once
#include "State.h"

namespace NCL {
	namespace CSC8503 {
		class PushdownState :
			public State
		{
		public:
			enum PushdownResult {
				Push, Pop, NoChange
			};
			PushdownState();
			virtual ~PushdownState();

			virtual PushdownResult OnUpdate(float dt, PushdownState **pushFunc) = 0;

			virtual void OnAwake() {} //By default do nothing
			virtual void OnSleep() {} //By default do nothing
		};
	}
}

