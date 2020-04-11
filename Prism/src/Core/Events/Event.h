#pragma once

#include <functional>

#define PR_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

namespace Prism {

	class Event
	{
	public:
		enum class Type {
			WindowClose, WindowResize,
			KeyPressed, KeyReleased,
			MouseMoved, MouseScrolled,
			MouseButtonPressed, MouseButtonReleased
		};
		virtual Type GetType() const = 0;

		// Try to handle event as type T
		// Returns false if T is the wrong type
		// Otherwise, the event is handled with function f
		template<typename T, typename F>
		bool Handle(const F& f)
		{
			if (GetType() != T::GetStaticType())
				return false;

			handled = f(static_cast<T&>(*this));
			return true;
		}

		bool handled = false;
	};

	// Define Static + Dynamic Type for check in Dispatch
#define DEFINE_EVENT_TYPE(type)\
	static Event::Type GetStaticType() { return type; }\
	virtual Event::Type GetType() const override { return type; }
}