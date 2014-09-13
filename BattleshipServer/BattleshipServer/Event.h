#pragma once
#include <memory>
enum EventTypes
{
	EVT_ERROR,
	EVT_NONE,
	EVT_OK,
	EVT_SUBMIT_NAME,
	EVT_SUBMIT_MAP,
	EVT_MY_TURN,
	EVT_SUBMIT_ATTACK,
	EVT_AttackResult,

	EVT_NEW_GAME,
	EVT_ADD_PLAYER_1_IN_GAME,
	EVT_ADD_PLAYER_2_IN_GAME,
	EVT_GAME_START,
	EVT_GAME_OVER,
	EVT_NEXT_GAME,
	EVT_ALL_OVER,
} ;

struct EventHeader
{
	EventTypes event_type_;
	PlayerNumber player_number_;
};

namespace Event
{

	struct ErrorEvent : public EventHeader
	{
		ErrorEvent()
		{
			event_type_ = EVT_ERROR;
			error_type_ = EVT_NONE;
		}
		int error_type_;
	};

	struct OKEvent : public EventHeader
	{
		OKEvent()
		{
			event_type_ = EVT_OK;
		}
	};

	struct SubmitNameEvent : public EventHeader
	{
		SubmitNameEvent()
		{
			event_type_ = EVT_SUBMIT_NAME;
			memset(name_, 0, sizeof(name_));
		}
		void SetName(wchar_t* name, int length)
		{
			memcpy(name_, name, sizeof(wchar_t) * min(MAX_NAME_LEN, length));
		}
		wchar_t name_[MAX_NAME_LEN];
	};

	struct GameStartEvent : public EventHeader
	{
		GameStartEvent()
		{
			event_type_ = EVT_GAME_START;
			game_number_ = -1;
		}
		GameNumber game_number_;
	};

	struct SubmitMapEvent : public EventHeader
	{
		SubmitMapEvent()
		{
			event_type_ = EVT_SUBMIT_MAP;
			memset(mMap, MI_EMPTY, sizeof(mMap));
		}
		char mMap[8][8];
	};

	struct MyTurnEvent : public EventHeader
	{
		MyTurnEvent()
		{
			event_type_ = EVT_MY_TURN;
		}
	};

	struct SubmitAttackEvent : public EventHeader
	{
		SubmitAttackEvent()
		{
			event_type_ = EVT_SUBMIT_ATTACK;
			x = y = -1;
		}
		int x, y;
	};

	struct AttackEvent : public EventHeader
	{
		AttackEvent()
		{
			event_type_ = EVT_AttackResult;
			x = y = -1;
			AttackResult_ = AR_NONE;
			isMine = false;
		}
		int x, y;
		bool isMine;
		AttackResult AttackResult_;
	};
	struct GameOverEvent : public EventHeader
	{
		GameOverEvent()
		{
			event_type_ = EVT_GAME_OVER;
			turns_ = -1;
			winner_ = -1;
			game_number_ = -1;
		}
		PlayerNumber winner_;
		int turns_;
		GameNumber game_number_;
	};

	struct NextGameEvent : public EventHeader
	{
		NextGameEvent()
		{
			event_type_ = EVT_NEXT_GAME;
			game_number_ = -1;
		}
		GameNumber game_number_;
	};

	struct AllOverEvent : public EventHeader
	{
		AllOverEvent()
		{
			event_type_ = EVT_ALL_OVER;
			win_count_ = -1;
			average_truns_ = NAN;
			game_number_ = -1;
		}
		int win_count_;
		float average_truns_;
		GameNumber game_number_;
	};
	
	struct NewGameEvent : public EventHeader
	{
		NewGameEvent()
		{
			event_type_ = EVT_NEW_GAME;
		}
	};
	
	struct AddPlayer1InGameEvent : public EventHeader
	{
		AddPlayer1InGameEvent()
		{
			event_type_ = EVT_ADD_PLAYER_1_IN_GAME;
			player_number_ = -1;
			game_number_ = -1;
		}
		GameNumber game_number_;
		PlayerNumber player_number_;
	};


	struct AddPlayer2InGameEvent : public EventHeader
	{
		AddPlayer2InGameEvent()
		{
			event_type_ = EVT_ADD_PLAYER_2_IN_GAME;
			player_number_ = -1;
			game_number_ = -1;
		}
		GameNumber game_number_;
		PlayerNumber player_number_;
	};
	
}