#pragma once
#include "stdafx.h"
#include "EventListener.h"
#include "Game.h"

typedef std::map<GameNumber, Game*> GameList;

class GameManager :
	public EventListener
{
public:
	static GameManager* GetInstance()
	{
		if (pInstance_ == nullptr)
		{
			pInstance_ = new GameManager();
		}
		return pInstance_;
	}
	static void FreeInstance()
	{
		if (pInstance_ != nullptr)
		{
			delete pInstance_;
			pInstance_ = nullptr;
		}
	}

	void Notify(EventHeader* event);

	std::list<GameNumber> GetWaitingGames(bool wannaNonFull = true);

	private:
		GameManager();
		~GameManager();


		static GameManager* pInstance_;
	GameList gameList_;
};

