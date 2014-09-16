#include "stdafx.h"
#include "PlayerManager.h"
#include "EventManager.h"


PlayerManager* PlayerManager::instance_ = nullptr;

PlayerManager::PlayerManager()
{
	playerList_.clear();
	EventManager::GetInstance()->AddEventListener(EVT_SUBMIT_NAME, this);
	EventManager::GetInstance()->AddEventListener(EVT_DISCONNECT, this);
}


PlayerManager::~PlayerManager()
{
	EventManager::GetInstance()->RemoveEventListener(this);
}


void PlayerManager::Notify(EventHeader* event)
{
	switch (event->event_type_)
	{
	case EVT_SUBMIT_NAME:
	{
		Event::SubmitNameEvent *recvEvent = (Event::SubmitNameEvent*)event;

		Player* player = new Player(recvEvent->name_, recvEvent->player_number_);
		playerList_.insert(PlayerList::value_type(recvEvent->player_number_, player));

		Event::OKEvent sendEvent;
		sendEvent.player_number_ = recvEvent->player_number_;
		EventManager::GetInstance()->Notify(&sendEvent);
	}break;
	case EVT_DISCONNECT:
	{
		Event::DisconnectEvent *recvEvent = (Event::DisconnectEvent*)event;

		auto player = playerList_.find(recvEvent->player_number_);
		if (player != playerList_.end())
		{
			delete player->second;
			playerList_.erase(player);
		}
	}break;
	default:
		break;
	}
}

std::list<PlayerNumber> PlayerManager::GetWaitingPlayers()
{
	std::list<PlayerNumber> ret;
	for (auto& player : playerList_)
	{
		if (player.second->GetState() == PS_WAIT)
		{
			ret.push_back(player.first);
		}
	}
	return ret;
}


PlayerNumber PlayerManager::GetNonUsedPlayerNumber()
{
	for (int i = 0; i < INT_MAX; i++)
	{
		if (playerList_.find(i) == playerList_.end())
			return i;
	}
}