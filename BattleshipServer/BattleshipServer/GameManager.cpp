#include "stdafx.h"
#include "GameManager.h"
#include "EventManager.h"

GameManager* GameManager::pInstance_ = nullptr;

GameManager::GameManager()
{
	EventManager::GetInstance()->AddEventListener(EVT_NEW_GAME, this);
	EventManager::GetInstance()->AddEventListener(EVT_ALL_OVER, this);
}


GameManager::~GameManager()
{
	EventManager::GetInstance()->RemoveEventListener(this);
}

void GameManager::Notify(EventHeader* event)
{
	switch (event->event_type_)
	{
	case EVT_NEW_GAME:
	{
		Game* game = new Game();
		GameNumber gameNumber = 0;

		while (true)
		{
			if (gameList_.find(gameNumber) != gameList_.end())
				break;
			gameNumber++;
		}

		gameList_.insert(GameList::value_type(gameNumber, game));
	}break;
	case EVT_ALL_OVER:
	{
		Event::AllOverEvent* recvEvent = (Event::AllOverEvent*)event;

		auto gameIt = gameList_.find(recvEvent->game_number_);
		if (gameIt != gameList_.end())
		{
			delete gameIt->second;
			gameList_.erase(gameIt);
		}

	}break;
	default:
		break;
	}
}