#include "GameStates.h"

#include "../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/PushdownState.h"

using namespace NCL;
using namespace CSC8503;

PushdownState::PushdownResult menuState::OnUpdate(float dt, PushdownState** newState)
{
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM1))
	{
		*newState = new TestState(g);
		return PushdownResult::Push;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM2))
	{
		*newState = new singlePlayerState(g);
		return PushdownResult::Push;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM3))
	{
		*newState = new twoPlayerState(g);
		return PushdownResult::Push;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM4) ||
		Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE) )
	{
		return PushdownResult::Pop;
	}
	g->UpdateGame(dt);
}

void menuState::OnAwake()
{
	g->initStartMenu();
}


PushdownState::PushdownResult singlePlayerState::OnUpdate(float dt, PushdownState** newState)
{
	g->UpdateGame(dt);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F1))
	{
		g->InitSingleCourse();
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F3))
	{
		return PushdownResult::Pop;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE))
	{
		return PushdownResult::Pop;
	}
	
	return PushdownResult::NoChange;
}

void singlePlayerState::OnAwake() 
{
	g->InitSingleCourse();
}


PushdownState::PushdownResult twoPlayerState::OnUpdate(float dt, PushdownState** newState)
{	
	g->UpdateGame(dt);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F1))
	{
		g->initDoubleCourse();
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F3))
	{
		return PushdownResult::Pop;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE))
	{
		return PushdownResult::Pop;
	}
	
	return PushdownResult::NoChange;
}

void twoPlayerState::OnAwake() 
{
	g->initDoubleCourse();
}


PushdownState::PushdownResult TestState::OnUpdate(float dt, PushdownState** newState)
{	
	g->UpdateGame(dt);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F1))
	{
		g->InitWorld();
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F3))
	{
		return PushdownResult::Pop;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE))
	{
		return PushdownResult::Pop;
	}
	
	return PushdownResult::NoChange;
}

void TestState::OnAwake()
{
	g->InitWorld();
}