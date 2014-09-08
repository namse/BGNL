#pragma once
#include <memory>
enum EventTypes
{
	EVENT_NONE = -1,

} ;

struct EventHeader
{
	EventTypes event_type_;
};

namespace Event
{
	//EX)
	/*struct FirstClickEvent : public EventHeader
	{
		FirstClickEvent()
		{
			event_type_ = EVENT_FIRST_CLICK;
		}
	};*/

}