#include "GameStateManager.h"


GameStateManager::GameStateManager()
{
	running = true;
	g = new TutorialGame;
	machine = new PushdownMachine(new menuState(g));
}

GameStateManager::~GameStateManager()
{
	delete g;
	delete machine;
}

void GameStateManager::Update(float dt)
{
	g->UpdateGame(dt);
	if (!machine->Update(dt))
	{
		running = false;
	}

}