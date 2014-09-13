#pragma once
#include "stdafx.h"
#include "EventListener.h"
class Game :
	public EventListener
{
public:
	Game(GameNumber game_number);
	~Game();

	void Notify(EventHeader* event);
	
	bool IsGameStart()
	{
		return isGameStart;
	}

	bool IsFull();

	bool IsPlayerInHere(PlayerNumber playerNumber)
	{
		return (playerNumber == player1_ || playerNumber == player2_);
	}
	
	bool IsGameAllOver()
	{
		return isGameAllOver_;
	}
private:

	void DropPlayer(bool isPlayer1);

	void DropDisconnectedPlayer()
	{
		DropPlayer(true);
		DropPlayer(false);
	}

	PlayerNumber GetOpponent(PlayerNumber playerNumber)
	{
		if (playerNumber == player1_)
			return player2_;
		else if (playerNumber == player2_)
			return player1_;
		return -1;
	}

	int GetFirstTurn()
	{
		if (rand() % 2)
			prev_trun_ = player1_;
		prev_trun_ = player2_;
		return prev_trun_;
	}

	int GetNextTurn()
	{
		prev_trun_ = GetOpponent(prev_trun_);
		return prev_trun_;
	}



	GameNumber game_number_;
	PlayerNumber player1_;
	PlayerNumber player2_;

	int try_count_;
	int prev_trun_;

	bool isGameStart;
	bool isGameAllOver_;

};

