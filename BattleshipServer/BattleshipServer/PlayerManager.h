#pragma once
#include "stdafx.h"
#include "Player.h"
#include "EventListener.h"
class PlayerManager : public  EventListener
{
public:
	static PlayerManager* GetInstance()
	{
		if (instance_ == nullptr)
		{
			instance_ = new PlayerManager();
		}
		return instance_;
	}

	static void FreeInstance()
	{
		if (instance_ != nullptr)
		{
			delete instance_;
			instance_ = nullptr;
		}
	}

	void Notify(EventHeader* event);

	std::list<PlayerNumber> GetWaitingPlayer();

	Player* GetPlayer(PlayerNumber playerNumber)
	{
		if (playerList_.find(playerNumber) != playerList_.end())
			return playerList_[playerNumber];
		return nullptr;
	}
private:
	PlayerManager();
	~PlayerManager();

private:
	static PlayerManager* instance_;

	typedef std::map<PlayerNumber, Player*> PlayerList;
	PlayerList playerList_;
};

