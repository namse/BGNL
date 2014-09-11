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
};

namespace Event
{

	struct ErrorEvent : public EventHeader
	{
		ErrorEvent()
		{
			event_type_ = EVT_ERROR;
			mErrorType = EVT_NONE;
		}
		int mErrorType;
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
			memset(mName, 0, sizeof(mName));
		}
		void SetName(wchar_t* name, int length)
		{
			memcpy(mName, name, sizeof(wchar_t) * min(MAX_NAME_LEN, length));
		}
		wchar_t mName[MAX_NAME_LEN];
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

	struct GameOverEvent : public EventHeader
	{
		GameOverEvent()
		{
			event_type_ = EVT_GAME_OVER;
			mTurns = -1;
			mIsWinner = false;
		}
		bool mIsWinner;
		int mTurns;
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
			mWinCount = -1;
			mAverageTruns = NAN;
		}
		int mWinCount;
		float mAverageTruns;
	};

}