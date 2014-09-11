#pragma once
#include <memory>
enum EventTypes
{
	EVT_ERROR = -1,
	EVT_NONE = 0,
	EVT_OK = 1,
	EVT_SUBMIT_NAME = 2,
	EVT_GAME_START = 3,
	EVT_SUBMIT_MAP = 4,
	EVT_MY_TURN = 5,
	EVT_SUBMIT_ATTACK = 6,
	EVT_ATTACK_RESULT = 7,
	EVT_GAME_OVER = 8,
	EVT_NEXT_GAME = 9,
	EVT_ALL_OVER = 10,
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
		}
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
			event_type_ = EVT_ATTACK_RESULT;
			x = y = -1;
			attack_result_ = AR_NONE;
		}
		int x, y;
		ATTACK_RESULT attack_result_;
	};
	struct GameOverEvent : public EventHeader
	{
		GameOverEvent()
		{
			event_type_ = EVT_GAME_OVER;
			turns_ = -1;
			is_winner_ = false;
		}
		bool is_winner_;
		int turns_;
	};

	struct NextGameEvent : public EventHeader
	{
		NextGameEvent()
		{
			event_type_ = EVT_NEXT_GAME;
		}
	};

	struct AllOverEvent : public EventHeader
	{
		AllOverEvent()
		{
			event_type_ = EVT_ALL_OVER;
			win_count_ = -1;
			average_truns_ = NAN;
		}
		int win_count_;
		float average_truns_;
	};

}