#pragma once

#include "Event.h"

namespace Prism {

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;
		DEFINE_EVENT_TYPE(Type::WindowClose)
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
			: m_Width(width), m_Height(height) {}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		DEFINE_EVENT_TYPE(Type::WindowResize)
	private:
		uint32_t m_Width, m_Height;
	};

}