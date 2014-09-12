#include "stdafx.h"
#include "Game.h"
#include "EventManager.h"
#include "PlayerManager.h"


Game::Game(GameNumber game_number)
	: player1_(-1), player2_(-2), game_number_(game_number), turns_(-1), try_count_(0), total_turns_(0)
	, win_count_1_(0), win_count_2_(0), isGameStart(false)
{
	EventManager::GetInstance()->AddEventListener(EVT_ADD_PLAYER_1_IN_GAME, this);
	EventManager::GetInstance()->AddEventListener(EVT_ADD_PLAYER_2_IN_GAME, this);
	EventManager::GetInstance()->AddEventListener(EVT_SUBMIT_ATTACK, this);
	EventManager::GetInstance()->AddEventListener(EVT_SUBMIT_MAP, this);
	EventManager::GetInstance()->AddEventListener(EVT_GAME_OVER, this);
	EventManager::GetInstance()->AddEventListener(EVT_NEXT_GAME, this);
	EventManager::GetInstance()->AddEventListener(EVT_GAME_START, this);
}


Game::~Game()
{
	EventManager::GetInstance()->RemoveEventListener(this);
}

bool Game::IsFull()
{
	DropDisconnectedPlayer();
	return PlayerManager::GetInstance()->GetPlayer(player1_) != nullptr
		&& PlayerManager::GetInstance()->GetPlayer(player1_) != nullptr;
}
void Game::DropPlayer(bool isPlayer1)
{
	if (isPlayer1)
	{
		auto player = PlayerManager::GetInstance()->GetPlayer(player2_);
		if (player != nullptr)
			player->SetState(PS_WAIT);
		player1_ = -1;
	}
	else
	{
		auto player = PlayerManager::GetInstance()->GetPlayer(player2_);
		if (player != nullptr)
			player->SetState(PS_WAIT);
		player2_ = -1;

	}
}


void Game::Notify(EventHeader* event)
{
	switch (event->event_type_)
	{
		if (isGameStart == false)
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
		auto player = PlayerManager::GetInstance()->GetPlayer(player1_);
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
		}

		if (event->player_number_ == player1_
			|| event->player_number_ == player2_)
		{


	case EVT_SUBMIT_ATTACK:
	{
		Event::SubmitAttackEvent* recvEvent = (Event::SubmitAttackEvent*)event;

		auto player_number = recvEvent->player_number_;
		auto oppo_number = GetOpponent(player_number);
		int x = recvEvent->x;
		int y = recvEvent->y;
		auto opponent_player = PlayerManager::GetInstance()->GetPlayer(oppo_number);
		if (opponent_player == nullptr)
		{
			//TODO

			break;
		}
		auto result = opponent_player->AttackAndGetResult(x, y);

		// 1. send result
		{
			Event::AttackEvent outEvent;
			outEvent.attack_result_ = result;
			outEvent.player_number_ = player_number;
			outEvent.x = x;
			outEvent.y = y;
			EventManager::GetInstance()->Notify(&outEvent);
		}
		// 2. check is game over

		if (opponent_player->IsGameEnd() == true)
		{
			Event::GameOverEvent outEvent;
			outEvent.game_number_ = game_number_;
			outEvent.winner_ = player_number;
			outEvent.turns_ = turns_;
			outEvent.player_number_ = player_number;
			EventManager::GetInstance()->Notify(&outEvent);

			outEvent.player_number_ = oppo_number;
			EventManager::GetInstance()->Notify(&outEvent);

			if (player1_ == player_number)
				win_count_1_++;
			else
				win_count_2_++;
		}
	}break;

	case EVT_SUBMIT_MAP:
	{
		Event::SubmitMapEvent * recvEvent = (Event::SubmitMapEvent *)event;
		auto player = PlayerManager::GetInstance()->GetPlayer(recvEvent->player_number_);
		auto oppo_player = PlayerManager::GetInstance()->GetPlayer(GetOpponent(recvEvent->player_number_));
		if (player == nullptr)
		{
			//TODO

			break;
		}

		player->SetMap(recvEvent->mMap);
		player->SetState(PS_ON_GAME);
		if (player->GetState() == PS_ON_GAME
			&& oppo_player->GetState() == PS_ON_GAME)
		{
			Event::GameStartEvent outEvent;
			outEvent.game_number_ = game_number_;
			outEvent.player_number_ = player1_;
			EventManager::GetInstance()->Notify(&outEvent);

			outEvent.player_number_ = player2_;
			EventManager::GetInstance()->Notify(&outEvent);
		}
	}break;

	case EVT_GAME_OVER:
	{
		Event::GameOverEvent* recvEvent = (Event::GameOverEvent*)event;

		total_turns_ += turns_;

		if (try_count_ >= MAX_GAME_COUNT)
		{
			Event::AllOverEvent outEvent;
			outEvent.average_truns_ = (float)total_turns_ / (float)try_count_;
			outEvent.game_number_ = game_number_;
			outEvent.win_count_ = win_count_1_;
			outEvent.player_number_ = player1_;
			EventManager::GetInstance()->Notify(&outEvent);

			outEvent.win_count_ = win_count_2_;
			outEvent.player_number_ = player2_;
			EventManager::GetInstance()->Notify(&outEvent);
		}

		else
		{
			Event::NextGameEvent outEvent;
			outEvent.game_number_ = game_number_;
			outEvent.player_number_ = player1_;
			EventManager::GetInstance()->Notify(&outEvent);

			outEvent.player_number_ = player2_;
			EventManager::GetInstance()->Notify(&outEvent);
		}
	}break;

	case EVT_NEXT_GAME:
	{
		Event::GameStartEvent outEvent;
		outEvent.game_number_ = game_number_;
		outEvent.player_number_ = player1_;
		EventManager::GetInstance()->Notify(&outEvent);

		outEvent.player_number_ = player2_;
		EventManager::GetInstance()->Notify(&outEvent);

	}break;

	case EVT_GAME_START:
	{
		isGameStart = true;
		try_count_++;
		turns_ = 0;
		auto player1 = PlayerManager::GetInstance()->GetPlayer(player1_);
		auto player2 = PlayerManager::GetInstance()->GetPlayer(player2_);

		if (player1 == nullptr ||
			player2 == nullptr)
		{
			//TODO

			break;
		}

		player1->SetState(PlayerState::PS_WAIT_MAP);
		player2->SetState(PlayerState::PS_WAIT_MAP);
	}break;
		}
	default:
		break;
	}
}