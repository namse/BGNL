#pragma once
#include "stdafx.h"
#include "Player.h"
class PlayerManager : public  EventListener
{
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


private:
	PlayerManager();
	~PlayerManager();

private:
	static PlayerManager* instance_;

	typedef std::map<PlayerNumber, Player*> PlayerList;
	PlayerList playerList_;
	
};

