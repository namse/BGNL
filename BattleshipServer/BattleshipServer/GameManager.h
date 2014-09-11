#pragma once
#include "stdafx.h"
#include "EventListener.h"
#include "Game.h"
class GameManager :
	public EventListener
{
public:
	GameManager();
	~GameManager();
	void Notify(EventHeader* event);

	typedef std::map<GameNumber, Game*> GameList;
	GameList gameList_;
};

