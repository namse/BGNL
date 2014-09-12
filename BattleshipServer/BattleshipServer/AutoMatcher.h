#pragma once
#include "Component.h"
class AutoMatcher :
	public Component
{
public:
	AutoMatcher();
	~AutoMatcher();

	void Notify(EventHeader* event);
	void Update(float dTime);
	void Render();

};

