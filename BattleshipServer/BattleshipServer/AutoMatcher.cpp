#include "stdafx.h"
#include "AutoMatcher.h"
#include "PlayerManager.h"
#include "EventManager.h"
#include "GameManager.h"
AutoMatcher::AutoMatcher()
{
}


AutoMatcher::~AutoMatcher()
{
}



void AutoMatcher::Notify(EventHeader* event)
{

}
void AutoMatcher::Update(float dTime)
{
	Component::Update(dTime);

	auto waitingPlayers = PlayerManager::GetInstance()->GetWaitingPlayers();

	while (waitingPlayers.size() >= 2)
	{
		auto player1 = waitingPlayers.front();
		waitingPlayers.pop_front();
		auto player2 = waitingPlayers.front();
		waitingPlayers.pop_front();

		{
			Event::NewGameEvent outEvent;
			EventManager::GetInstance()->Notify(&outEvent);
		}

		auto games = GameManager::GetInstance()->GetWaitingGames();
		int gameNumber = games.front();
		{
			Event::AddPlayer1InGameEvent outEvent;
			outEvent.player_number_ = player1;
			outEvent.game_number_ = gameNumber;
			EventManager::GetInstance()->Notify(&outEvent);
		}
		{
			Event::AddPlayer2InGameEvent outEvent;
			outEvent.player_number_ = player2;
			outEvent.game_number_ = gameNumber;
			EventManager::GetInstance()->Notify(&outEvent);
		}
		{
			Event::GameStartEvent outEvent;
			outEvent.game_number_ = gameNumber;
			outEvent.player_number_ = player2;
			EventManager::GetInstance()->Notify(&outEvent);

			outEvent.player_number_ = player1;
			EventManager::GetInstance()->Notify(&outEvent);
		}
	}
}

void AutoMatcher::Render()
{
	Component::Render();
}
