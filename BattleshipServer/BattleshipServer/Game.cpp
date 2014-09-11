#include "stdafx.h"
#include "Game.h"
#include "EventManager.h"


Game::Game()
{
}


Game::~Game()
{
	EventManager::GetInstance()->RemoveEventListener(this);
}

void Game::Notify(EventHeader* event)
{

}