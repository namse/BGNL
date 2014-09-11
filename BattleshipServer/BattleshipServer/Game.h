#pragma once
#include "stdafx.h"
#include "EventListener.h"
class Game :
	public EventListener
{
public:
	Game();
	~Game();

	void Notify(EventHeader* event);
};

