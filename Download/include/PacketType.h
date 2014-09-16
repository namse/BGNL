#pragma once
#include <math.h>
#include <memory.h>

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

#define MAX_NAME_LEN	16

#define MAP_WIDTH 8
#define MAP_HEIGHT 8
#define MAP_SIZE (MAP_WIDTH * MAP_HEIGHT)


#define AIRCRAFT_LENGTH 5
#define BATTLESHIP_LENGTH 4
#define CRUISER_LENGTH 3
#define DESTROYER_LENGTH 2

struct Coord
{
	Coord()
	{
		mX = mY = -1;
	}
	Coord(int x, int y)
	{
		mX = x, mY = y;
	}
	void Intigerize()
	{
		if (mX >= 'A' && mX <= 'Z')
			mX -= 'A';
		if (mX >= 'a' && mX <= 'z')
			mX -= 'a';
		if (mX >= '1' && mX <= '9')
			mX -= '1';
		if (mY >= 'A' && mY <= 'Z')
			mY -= 'A';
		if (mY >= 'a' && mY <= 'z')
			mY -= 'a';
		if (mY >= '1' && mY <= '9')
			mY -= '1';
	}
	char mX, mY;
};

#define SHNIPS_TOTAL_LENGTH (AIRCRAFT_LENGTH + BATTLESHIP_LENGTH + CRUISER_LENGTH + DESTROYER_LENGTH + DESTROYER_LENGTH)

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

enum ErrorTypes
{
	ET_UNKNOWN,
	ET_OK,
	ET_OPPOSITION_DISCONNECTED,
	ET_DUPLICATED_NAME,
	ET_INVALID_MAP,
	ET_INVALID_ATTACK,
};

enum MapInfo
{
	MI_EMPTY = 0,
	MI_AIRCRAFT = 1,
	MI_BATTLESHIP = 2,
	MI_CRUISER = 3,
	MI_DESTROYER_1 = 4,
	MI_DESTROYER_2 = 5,
};

enum AttackResultTypes
{
	AR_NONE = 0,
	AR_MISS = 1,
	AR_HIT = 2,
	AR_DESTROY_AIRCRAFT = 3,
	AR_DESTROY_BATTLESHIP = 4,
	AR_DESTROY_CRUISER = 5,
	AR_DESTROY_DESTROYER = 6,
};

enum MapDataTypes
{
	MD_NONE,
	MD_AIRCRAFT,
	MD_BATTLESHIP,
	MD_CRUISER,
	MD_DESTROYER1,
	MD_DESTROYER2,
	MD_END,
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
			mErrorType = ET_UNKNOWN;
		}
		short mErrorType;
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
		int mStudentID;
	};

	struct GameStartResult : public PacketHeader
	{
		GameStartResult()
		{
			mSize = sizeof(GameStartResult);
			mType = PKT_SC_GAME_START;
		}
		wchar_t mOppositionName[MAX_NAME_LEN];
		int mOppositionStudentID;
	};

	struct SubmitMapRequest : public PacketHeader
	{
		SubmitMapRequest()
		{
			mSize = sizeof(SubmitMapRequest);
			mType = PKT_CS_SUBMIT_MAP;
			memset(mMap, 0, sizeof(mMap));
		}
		char mMap[MAP_SIZE];
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
		}
		Coord mCoord;
	};

	struct AttackResult : public PacketHeader
	{
		AttackResult()
		{
			mSize = sizeof(AttackResult);
			mType = PKT_SC_ATTACK_RESULT;
			mAttackResult = AR_NONE;
			mIsMine = false;
		}
		Coord mCoord;
		short mAttackResult;
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