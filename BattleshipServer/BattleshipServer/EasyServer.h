#pragma once
#include "stdafx.h"
class EasyServer
{
public:
	static EasyServer* GetInstance()
	{
		if (pInstance_ == nullptr)
		{
			pInstance_ = new EasyServer;
		}
		return pInstance_;
	}
	static void FreeInstance()
	{
		if (pInstance_ != nullptr)
		{
			delete pInstance_;
			pInstance_ = nullptr;
		}
	}
	bool Run();
private:
	EasyServer();
	~EasyServer();

	bool InitAccepThread();


	static EasyServer* pInstance_;
	HANDLE hCAThread;
};

unsigned int WINAPI ClientAcceptThread(LPVOID lpParam);
