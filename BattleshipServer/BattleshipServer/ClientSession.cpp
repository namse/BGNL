#include "stdafx.h"
#include "ClientSession.h"
#include "ClientManager.h"
#include "EventManager.h"
#include "Event.h"

//@{ Handler Helper

typedef void (*HandlerFunc)(ClientSession* session, PacketHeader& pktBase) ;

static HandlerFunc HandlerTable[PKT_MAX];

static void DefaultHandler(ClientSession* session, PacketHeader& pktBase)
{
	assert(false);
	session->Disconnect();
}

struct InitializeHandlers
{
	InitializeHandlers()
	{
		for (int i = 0; i < PKT_MAX; ++i)
			HandlerTable[i] = DefaultHandler;
	}
} _init_handlers_;

struct RegisterHandler
{
	RegisterHandler(int pktType, HandlerFunc handler)
	{
		HandlerTable[pktType] = handler;
	}
};

#define REGISTER_HANDLER(PKT_TYPE)	\
	static void Handler_##PKT_TYPE(ClientSession* session, PacketHeader& pktBase); \
	static RegisterHandler _register_##PKT_TYPE(PKT_TYPE, Handler_##PKT_TYPE); \
	static void Handler_##PKT_TYPE(ClientSession* session, PacketHeader& pktBase)

//@}

ClientSession::ClientSession(SOCKET sock)
	: mConnected(false), mLogon(false), mSocket(sock), mPlayerId(sock), mOverlappedRequested(0)
	, mPosX(0), mPosY(0), mPosZ(0), mDbUpdateCount(0), mSendBuffer(BUFSIZE), mRecvBuffer(BUFSIZE)
{
	memset(&mClientAddr, 0, sizeof(SOCKADDR_IN));
	memset(mPlayerName, 0, sizeof(mPlayerName));
	EventManager::GetInstance()->AddEventListener(EVT_ERROR, this);
	EventManager::GetInstance()->AddEventListener(EVT_OK, this);
	EventManager::GetInstance()->AddEventListener(EVT_GAME_START, this);
	EventManager::GetInstance()->AddEventListener(EVT_MY_TURN, this);
	EventManager::GetInstance()->AddEventListener(EVT_AttackResult, this);
	EventManager::GetInstance()->AddEventListener(EVT_GAME_OVER, this);
	EventManager::GetInstance()->AddEventListener(EVT_NEXT_GAME, this);
	EventManager::GetInstance()->AddEventListener(EVT_ALL_OVER, this);
}

ClientSession::~ClientSession()
{
	EventManager::GetInstance()->RemoveEventListener(this);
}


bool ClientSession::OnConnect(SOCKADDR_IN* addr)
{
	memcpy(&mClientAddr, addr, sizeof(SOCKADDR_IN));

	/// 소켓을 넌블러킹으로 바꾸고
	u_long arg = 1;
	ioctlsocket(mSocket, FIONBIO, &arg);

	/// nagle 알고리즘 끄기
	int opt = 1;
	setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(int));

	printf("[DEBUG] Client Connected: IP=%s, PORT=%d\n", inet_ntoa(mClientAddr.sin_addr), ntohs(mClientAddr.sin_port));

	mConnected = true;

	return PostRecv();
}

bool ClientSession::PostRecv()
{
	if (!IsConnected())
		return false;

	DWORD recvbytes = 0;
	DWORD flags = 0;
	WSABUF buf;
	buf.len = (ULONG)mRecvBuffer.GetFreeSpaceSize();
	buf.buf = (char*)mRecvBuffer.GetBuffer();

	memset(&mOverlappedRecv, 0, sizeof(OverlappedIO));
	mOverlappedRecv.mObject = this;

	/// 비동기 입출력 시작
	if (SOCKET_ERROR == WSARecv(mSocket, &buf, 1, &recvbytes, &flags, &mOverlappedRecv, RecvCompletion))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
			return false;
	}

	IncOverlappedRequest();

	return true;
}

void ClientSession::Disconnect()
{
	if (!IsConnected())
		return;

	printf("[DEBUG] Client Disconnected: IP=%s, PORT=%d\n", inet_ntoa(mClientAddr.sin_addr), ntohs(mClientAddr.sin_port));

	/// 즉각 해제

	LINGER lingerOption;
	lingerOption.l_onoff = 1;
	lingerOption.l_linger = 0;

	/// no TCP TIME_WAIT
	if (SOCKET_ERROR == setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&lingerOption, sizeof(LINGER)))
	{
		printf_s("[DEBUG] setsockopt linger option error: %d\n", GetLastError());
		return;
	}

	closesocket(mSocket);

	mConnected = false;
}

void ClientSession::OnRead(size_t len)
{
	mRecvBuffer.Commit(len);

	/// 패킷 파싱하고 처리
	while (true)
	{
		/// 패킷 헤더 크기 만큼 읽어와보기
		PacketHeader header;
		if (false == mRecvBuffer.Peek((char*)&header, sizeof(PacketHeader)))
			return;

		/// 패킷 완성이 되는가? 
		if (mRecvBuffer.GetStoredSize() < header.mSize)
			return;


		if (header.mType >= PKT_MAX || header.mType <= PKT_NONE)
		{
			Disconnect();
			return;
		}

		/// packet dispatch...
		HandlerTable[header.mType](this, header);
	}
}


bool ClientSession::SendRequest(PacketHeader* pkt)
{
	if (!IsConnected())
		return false;

	/// Send 요청은 버퍼에 쌓아놨다가 한번에 보낸다.
	if (false == mSendBuffer.Write((char*)pkt, pkt->mSize))
	{
		/// 버퍼 용량 부족인 경우는 끊어버림
		Disconnect();
		return false;
	}

	return true;

}

bool ClientSession::SendFlush()
{
	if (!IsConnected())
		return false;

	/// 보낼 데이터가 없으면 그냥 리턴
	if (mSendBuffer.GetContiguiousBytes() == 0)
		return true;

		DWORD sendbytes = 0;
	DWORD flags = 0;

	WSABUF buf;
	buf.len = (ULONG)mSendBuffer.GetContiguiousBytes();
	buf.buf = (char*)mSendBuffer.GetBufferStart();

	memset(&mOverlappedSend, 0, sizeof(OverlappedIO));
	mOverlappedSend.mObject = this;

	// 비동기 입출력 시작
	if (SOCKET_ERROR == WSASend(mSocket, &buf, 1, &sendbytes, flags, &mOverlappedSend, SendCompletion))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
			return false;
	}

	IncOverlappedRequest();

	//assert(buf.len == sendbytes);*/

	return true;
}

void ClientSession::OnWriteComplete(size_t len)
{
	/// 보내기 완료한 데이터는 버퍼에서 제거
	mSendBuffer.Remove(len);
}

bool ClientSession::Broadcast(PacketHeader* pkt)
{
	if ( !SendRequest(pkt) )
		return false ;

	if ( !IsConnected() )
		return false ;

	GClientManager->BroadcastPacket(this, pkt) ;

	return true ;
}

void ClientSession::OnTick()
{
	/// 클라별로 주기적으로 해야될 일은 여기에

}


void ClientSession::Notify(EventHeader* event)
{
	if (event->player_number_ == mPlayerId)
	{
		switch (event->event_type_)
		{
		case EVT_ERROR:
		{
			Event::ErrorEvent* recvEvent = (Event::ErrorEvent*)event;
			Packet::ErrorResult outPacket;
			outPacket.mErrorType = recvEvent->event_type_;
			SendRequest(&outPacket);

		}break;
		case EVT_OK:
		{
			Packet::OKResult outPacket;
			SendRequest(&outPacket);
		}break;
		case EVT_GAME_START:
		{
			Packet::GameStartResult outPacket;
			SendRequest(&outPacket);
		}break;
		case EVT_MY_TURN:
		{
			Packet::MyTurnResult outPacket;
			SendRequest(&outPacket);
		}break;
		case EVT_AttackResult:
		{
			Event::AttackEvent* recvEvent = (Event::AttackEvent*)event;

			Packet::AttackResult outPacket;
			outPacket.x = recvEvent->x;
			outPacket.y = recvEvent->y;
			outPacket.mAttackResult = recvEvent->event_type_;
			SendRequest(&outPacket);
		}break;
		case EVT_GAME_OVER:
		{
			Event::GameOverEvent* recvEvent = (Event::GameOverEvent*)event;

			Packet::GameOverResult outPacket;
			outPacket.mIsWinner = (recvEvent->winner_ == mPlayerId);
			outPacket.mTurns = recvEvent->turns_;
			SendRequest(&outPacket);
		}break;
		case EVT_NEXT_GAME:
		{
			Packet::NextGameResult outPacket;
			SendRequest(&outPacket);
		}break;
		case EVT_ALL_OVER:
		{
			Event::AllOverEvent* recvEvent = (Event::AllOverEvent*)event;

			Packet::AllOverResult outPacket;
			outPacket.mAverageTruns = recvEvent->average_truns_;
			outPacket.mWinCount = recvEvent->win_count_;
			SendRequest(&outPacket);
		}break;
		default:
			break;
		}
	}
}

///////////////////////////////////////////////////////////

void CALLBACK RecvCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	ClientSession* fromClient = static_cast<OverlappedIO*>(lpOverlapped)->mObject ;
	
	fromClient->DecOverlappedRequest() ;

	if ( !fromClient->IsConnected() )
		return ;

	/// 에러 발생시 해당 세션 종료
	if ( dwError || cbTransferred == 0 )
	{
		fromClient->Disconnect() ;
		return ;
	}

	/// 받은 데이터 처리
	fromClient->OnRead(cbTransferred) ;

	/// 다시 받기
	if ( false == fromClient->PostRecv() )
	{
		fromClient->Disconnect() ;
		return ;
	}
}


void CALLBACK SendCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	ClientSession* fromClient = static_cast<OverlappedIO*>(lpOverlapped)->mObject ;

	fromClient->DecOverlappedRequest() ;

	if ( !fromClient->IsConnected() )
		return ;

	/// 에러 발생시 해당 세션 종료
	if ( dwError || cbTransferred == 0 )
	{
		fromClient->Disconnect() ;
		return ;
	}

	fromClient->OnWriteComplete(cbTransferred) ;

}


//////////////////////////////////////////////////////////////////

REGISTER_HANDLER(PKT_CS_SUBMIT_NAME)
{
	Packet::SubmitNameRequest inPacket = static_cast<Packet::SubmitNameRequest&>(pktBase);
	session->HandleSubmitNameRequest(inPacket);
}
void ClientSession::HandleSubmitNameRequest(Packet::SubmitNameRequest& inPacket)
{
	mRecvBuffer.Read((char*)&inPacket, inPacket.mSize);


	Event::SubmitNameEvent event;
	event.player_number_ = mPlayerId;
	EventManager::GetInstance()->Notify(&event);
}

REGISTER_HANDLER(PKT_CS_SUBMIT_MAP)
{
	Packet::SubmitMapRequest inPacket = static_cast<Packet::SubmitMapRequest&>(pktBase);
	session->HandleSubmitMapRequest(inPacket);
}
void ClientSession::HandleSubmitMapRequest(Packet::SubmitMapRequest& inPacket)
{
	mRecvBuffer.Read((char*)&inPacket, inPacket.mSize);

	Event::SubmitMapEvent event;
	event.player_number_ = mPlayerId;
	EventManager::GetInstance()->Notify(&event);
}

REGISTER_HANDLER(PKT_CS_SUBMIT_ATTACK)
{
	Packet::SubmitAttackRequest inPacket = static_cast<Packet::SubmitAttackRequest&>(pktBase);
	session->HandleSubmitAttackRequest(inPacket);
}
void ClientSession::HandleSubmitAttackRequest(Packet::SubmitAttackRequest& inPacket)
{
	mRecvBuffer.Read((char*)&inPacket, inPacket.mSize);

	Event::SubmitAttackEvent event;
	event.player_number_ = mPlayerId;
	EventManager::GetInstance()->Notify(&event);
}
