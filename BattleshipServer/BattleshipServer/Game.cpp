#include "stdafx.h"
#include "Game.h"
#include "EventManager.h"
#include "PlayerManager.h"


Game::Game(GameNumber game_number)
	: player1_(-1), player2_(-2), game_number_(game_number), isGameStart(false), try_count_(0), isGameAllOver_(false)
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

	case EVT_ADD_PLAYER_1_IN_GAME:
	{
		Event::AddPlayer1InGameEvent* recvEvent = (Event::AddPlayer1InGameEvent*) event;
		if (isGameStart == true)
		{
			std::cout << "Already Start Game\n";

			//TODO

			break;
		}
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
		if (isGameStart == true)
		{
			std::cout << "Already Start Game\n";

			//TODO

			break;
		}
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

	case EVT_SUBMIT_ATTACK:
	{
		Event::SubmitAttackEvent* recvEvent = (Event::SubmitAttackEvent*)event;
		if (recvEvent->player_number_ == player1_
			|| recvEvent->player_number_ == player2_)
		{
			auto player_number = recvEvent->player_number_;
			auto oppo_number = GetOpponent(player_number);
			auto coord = recvEvent->coord_;
			auto player = PlayerManager::GetInstance()->GetPlayer(player_number);
			auto opponent_player = PlayerManager::GetInstance()->GetPlayer(oppo_number);
			if (player == nullptr || opponent_player == nullptr)
			{
				//TODO

				break;
			}
			
			player->CheckTurn();

			// 0, send OK
			{
				Event::OKEvent outEvent;
				outEvent.player_number_ = recvEvent->player_number_;
				EventManager::GetInstance()->Notify(&outEvent);
			}
			auto result = opponent_player->AttackAndGetResult(coord);

			// 1. send result
			{
				{
					Event::AttackEvent outEvent;
					outEvent.AttackResult_ = result;
					outEvent.coord_ = coord;

					outEvent.player_number_ = player_number;
					outEvent.isMine = true;
					EventManager::GetInstance()->Notify(&outEvent);


					outEvent.player_number_ = oppo_number;
					outEvent.isMine = false;
					EventManager::GetInstance()->Notify(&outEvent);	
				}
			}
			// 2. check is game over

			if (opponent_player->IsGameEnd() == true)
			{
				player->AddWinCount();
				player->AddWinTotalTurns();

				Event::GameOverEvent outEvent;
				outEvent.game_number_ = game_number_;
				outEvent.winner_ = player_number;
				outEvent.turns_ = player->GetTurns();
				EventManager::GetInstance()->Notify(&outEvent);

			}

			// 3. or Next Turn
			else 
			{
				Event::MyTurnEvent outEvent;

				outEvent.player_number_ = GetNextTurn();
				EventManager::GetInstance()->Notify(&outEvent);
			}
		}
	}break;

	case EVT_SUBMIT_MAP:
	{
		Event::SubmitMapEvent * recvEvent = (Event::SubmitMapEvent *)event;
		if (recvEvent->player_number_ == player1_
			|| recvEvent->player_number_ == player2_)
		{
			auto player = PlayerManager::GetInstance()->GetPlayer(recvEvent->player_number_);
			auto oppo_player = PlayerManager::GetInstance()->GetPlayer(GetOpponent(recvEvent->player_number_));
			if (player == nullptr || oppo_player == nullptr)
			{
				//TODO

				break;
			}

			player->SetMap(recvEvent->mMap);
			player->SetState(PS_ON_GAME);
			{
				Event::OKEvent outEvent;
				outEvent.player_number_ = recvEvent->player_number_;
				EventManager::GetInstance()->Notify(&outEvent);
			}
			if (player->GetState() == PS_ON_GAME
				&& oppo_player->GetState() == PS_ON_GAME)
			{
				Event::MyTurnEvent outEvent;

				outEvent.player_number_ = GetFirstTurn();
				EventManager::GetInstance()->Notify(&outEvent);
			}
		}
	}break;

	case EVT_GAME_OVER:
	{
		Event::GameOverEvent* recvEvent = (Event::GameOverEvent*)event;
		if (recvEvent->game_number_ == game_number_)
		{

			if (try_count_ >= MAX_GAME_COUNT)
			{
				Event::AllOverEvent outEvent;
				outEvent.game_number_ = game_number_;
				EventManager::GetInstance()->Notify(&outEvent);
				isGameAllOver_ = true;

			}

			else
			{
				Event::NextGameEvent outEvent;
				outEvent.game_number_ = game_number_;
				EventManager::GetInstance()->Notify(&outEvent);
			}
		}
	}break;

	case EVT_NEXT_GAME:
	{
		Event::NextGameEvent * recvEvent = (Event::NextGameEvent*)event;
		if (recvEvent->game_number_ == game_number_)
		{
			try_count_++;
			auto player1 = PlayerManager::GetInstance()->GetPlayer(player1_);
			auto player2 = PlayerManager::GetInstance()->GetPlayer(player2_);

			if (player1 == nullptr ||
				player2 == nullptr)
			{
				//TODO

				break;
			}
			player1->InitTurns();
			player2->InitTurns();

			player1->SetState(PlayerState::PS_WAIT_MAP);
			player2->SetState(PlayerState::PS_WAIT_MAP);
		}
	}break;

	case EVT_GAME_START:
	{
		Event::GameStartEvent * recvEvent = (Event::GameStartEvent*)event;
		if (recvEvent->game_number_ == game_number_)
		{
			isGameStart = true;
			try_count_++;
			auto player1 = PlayerManager::GetInstance()->GetPlayer(player1_);
			auto player2 = PlayerManager::GetInstance()->GetPlayer(player2_);

			if (player1 == nullptr ||
				player2 == nullptr)
			{
				//TODO

				break;
			}
			player1->InitTurns();
			player2->InitTurns();

			player1->SetState(PlayerState::PS_WAIT_MAP);
			player2->SetState(PlayerState::PS_WAIT_MAP);
		}
	}break;
	default:
		break;
	}
}