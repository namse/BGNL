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


	GameNumber game_number_;
	PlayerNumber player1_;
	PlayerNumber player2_;
	int turns_;
	int try_count_;
	int total_turns_;
	int win_count_1_;
	int win_count_2_;

	bool isGameStart;

};

