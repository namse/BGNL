#include "stdafx.h"
#include "Game.h"
#include "EventManager.h"
#include "PlayerManager.h"


Game::Game()
	: player1_(-1), player2_(-2)
{
	EventManager::GetInstance()->AddEventListener(EVT_ADD_PLAYER_1_IN_GAME, this);
	EventManager::GetInstance()->AddEventListener(EVT_ADD_PLAYER_2_IN_GAME, this);
	EventManager::GetInstance()->AddEventListener(EVT_GAME_START, this);
	EventManager::GetInstance()->AddEventListener(EVT_GAME_OVER, this);
	EventManager::GetInstance()->AddEventListener(EVT_NEXT_GAME, this);
}


Game::~Game()
{
	EventManager::GetInstance()->RemoveEventListener(this);
}

void Game::Notify(EventHeader* event)
{
	switch (event->event_type_)
	{
	case EVT_ADD_PLAYER_1_IN_GAME:
	{
		Event::AddPlayer1InGameEvent* recvEvent = (Event::AddPlayer1InGameEvent*) event;
		if (player1_ != -1)
		{
			auto player = PlayerManager::GetInstance()->GetPlayer(player1_);
			if (player != nullptr)
			{
				player->SetState(PS_WAIT);
			}
		}
		player1_ = recvEvent->player_number_;
		auto player = PlayerManager::GetInstance()->GetPlayer( player1_ );
		if (player != nullptr)
		{
			player->SetState(PS_READY);
		}
	}break;

	case EVT_ADD_PLAYER_2_IN_GAME:
	{
		Event::AddPlayer2InGameEvent* recvEvent = (Event::AddPlayer2InGameEvent*) event;
		if (player2_ != -1)
		{
			auto player = PlayerManager::GetInstance()->GetPlayer(player2_);
			if (player != nullptr)
			{
				player->SetState(PS_WAIT);
			}
		}
		player2_ = recvEvent->player_number_;
		auto player = PlayerManager::GetInstance()->GetPlayer(player2_);
		if (player != nullptr)
		{
			player->SetState(PS_READY);
		}
	}break;
	default:
		break;
	}
}