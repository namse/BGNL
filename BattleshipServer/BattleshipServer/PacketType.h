#pragma once
#include "stdafx.h"

#define MAX_CHAT_LEN	1024

#define MAX_NAME_LEN	30
#define MAX_COMMENT_LEN	40

enum PacketTypes
{
	PKT_NONE = 0,

	PKT_MAX = 1024
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

	//ex)
	/*
	struct VoteCompleteRequest : public PacketHeader
	{
		VoteCompleteRequest()
		{
			mSize = sizeof(VoteCompleteRequest);
			mType = PKT_VOTE_COMPLETE;
			objectLength = 0;
			memset(object, -1, sizeof(object));
		}
		char objectLength;
		char object[1 + WORK_COUNT];
	};*/

}
#pragma pack(pop)