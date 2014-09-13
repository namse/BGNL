#include "stdafx.h"
#include "GameManager.h"
#include "EventManager.h"

GameManager* GameManager::pInstance_ = nullptr;

GameManager::GameManager()
{
	EventManager::GetInstance()->AddEventListener(EVT_NEW_GAME, this);
}


GameManager::~GameManager()
{
	EventManager::GetInstance()->RemoveEventListener(this);
}
void GameManager::RemoveOverGames()
{
	for (auto gameIt = gameList_.begin(); gameIt != gameList_.end();)
	{
		if (gameIt->second->IsGameAllOver())
		{
			delete gameIt->second;
			gameIt = gameList_.erase(gameIt);
		}
		else
			gameIt++;
	}
}
void GameManager::Notify(EventHeader* event)
{
	switch (event->event_type_)
	{
	case EVT_NEW_GAME:
	{
		GameNumber gameNumber = 0;

		while (true)
		{
			if (gameList_.find(gameNumber) == gameList_.end())
				break;
			gameNumber++;
		}

		Game* game = new Game(gameNumber);
		gameList_.insert(GameList::value_type(gameNumber, game));
	}break;
	default:
		break;
	}
}



std::list<GameNumber> GameManager::GetWaitingGames(bool wannaNonFull)
{
	std::list<GameNumber> ret;
	for (auto& game : gameList_)
	{
		if (game.second->IsGameStart() == false)
		{
			if (wannaNonFull == true)
			{
				if (game.second->IsFull() == false)
				{
					ret.push_back(game.first);
				}
			}
			else
			{
				ret.push_back(game.first);
			}
		}
	}
	return ret;
}