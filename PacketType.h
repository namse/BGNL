#pragma once
#include <math.h>
#define MAX_NAME_LEN	16

enum PacketTypes
{
	// SC : Server -> Client
	// you know, CS means from client to server
	PKT_SC_ERROR = -1,
	PKT_NONE = 0,
	PKT_SC_OK = 1,
	PKT_CS_SUBMIT_NAME = 2,
	PKT_SC_GAME_START = 3,
	PKT_CS_SUBMIT_MAP = 4,
	PKT_SC_MY_TURN = 5,
	PKT_CS_SUBMIT_ATTACK = 6,
	PKT_SC_ATTACK_RESULT = 7,
	PKT_SC_GAME_OVER = 8,
	PKT_SC_NEXT_GAME = 9,
	PKT_SC_ALL_OVER = 10,
	
	PKT_MAX = 1024
};

enum ERROR_TYPE
{
	ET_NONE = 0,
	ET_OPPOSITION_DISCONNECTED = 1,
	ET_DUPLICATED_NAME = 2,
	ET_INVALID_MAP = 3,
	ET_INVALID_ATTACK = 4,
};

enum MAP_INFO
{
	MI_EMPTY = 0,
	MI_AIRCRAFT = 1,
	MI_BATTLESHIP = 2,
	MI_CRUSIER = 3,
	MI_DESTORYER_1 = 4,
	MI_DESTORYER_2 = 5,
};

enum ATTACK_RESULT
{
	AR_NONE = 0,
	AR_MISS = 1,
	AR_HIT = 2,
	AR_DESTORY_AIRCRAFT = 3,
	AR_DESTORY_BATTLESHIP = 4,
	AR_DESTORY_CRUSIER = 5,
	AR_DESTORY_DESTORYER = 6,
};


#pragma pack(push, 1)

struct PacketHeader
{
	PacketHeader() : mSize(0), mType(PKT_NONE) 	{}
	short mType; 
	short mSize;
};
namespace Packet
{
	// SC has suffix 'Result'
	// also CS has suffix 'Request 


	struct ErrorResult : public PacketHeader
	{
		ErrorResult()
		{
			mSize = sizeof(ErrorResult);
			mType = PKT_SC_ERROR;
			mErrorType = ET_NONE;
		}
		int mErrorType;
	};

	struct OKResult : public PacketHeader
	{
		OKResult()
		{
			mSize = sizeof(OKResult);
			mType = PKT_SC_OK;
		}
	};

	struct SubmitNameRequest : public PacketHeader
	{
		SubmitNameRequest()
		{
			mSize = sizeof(SubmitNameRequest);
			mType = PKT_CS_SUBMIT_NAME;
			memset(mName, 0, sizeof(mName));
		}
		void SetName(wchar_t* name, int length)
		{
			memcpy(mName, name, sizeof(wchar_t) * min(MAX_NAME_LEN, length));
		}
		wchar_t mName[MAX_NAME_LEN];
	};

	struct GameStartResult : public PacketHeader
	{
		GameStartResult()
		{
			mSize = sizeof(GameStartResult);
			mType = PKT_SC_GAME_START;
		}
	};

	struct SubmitMapRequest : public PacketHeader
	{
		SubmitMapRequest()
		{
			mSize = sizeof(SubmitMapRequest);
			mType = PKT_CS_SUBMIT_MAP;
			memset(mMap, MI_EMPTY, sizeof(mMap));
		}
		char mMap[8][8];
	};

	struct MyTurnResult : public PacketHeader
	{
		MyTurnResult()
		{
			mSize = sizeof(MyTurnResult);
			mType = PKT_SC_MY_TURN;
		}
	};

	struct SubmitAttackRequest : public PacketHeader
	{
		SubmitAttackRequest()
		{
			mSize = sizeof(SubmitAttackRequest);
			mType = PKT_CS_SUBMIT_ATTACK;
			x = y = -1;
		}
		int x, y;
	};

	struct AttackResult : public PacketHeader
	{
		AttackResult()
		{
			mSize = sizeof(AttackResult);
			mType = PKT_SC_ATTACK_RESULT;
			x = y = -1;
			mAttackResult = AR_NONE;
			mIsMine = false;
		}
		int x, y, mAttackResult;
		bool mIsMine;
	};

	struct GameOverResult : public PacketHeader
	{
		GameOverResult()
		{
			mSize = sizeof(GameOverResult);
			mType = PKT_SC_GAME_OVER;
			mTurns = -1;
			mIsWinner = false;
		}
		bool mIsWinner;
		int mTurns;
	};

	struct NextGameResult : public PacketHeader
	{
		NextGameResult()
		{
			mSize = sizeof(NextGameResult);
			mType = PKT_SC_NEXT_GAME;
		}
	};

	struct AllOverResult : public PacketHeader
	{
		AllOverResult()
		{
			mSize = sizeof(AllOverResult);
			mType = PKT_SC_ALL_OVER;
			mWinCount = -1;
			mAverageTruns = NAN;
		}
		int mWinCount;
		float mAverageTruns;
	};
}
#pragma pack(pop)