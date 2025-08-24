#pragma once

#include <Nano/Nano.hpp>

#include <Obsidian/Maths/Structs.hpp>

#include <cstdint>

namespace Mario
{

	////////////////////////////////////////////////////////////////////////////////////
	// Preprocessor
	////////////////////////////////////////////////////////////////////////////////////
    #if defined(NANO_PLATFORM_WINDOWS)
        #define MM_PLATFORM_WINDOWS
    #elif defined(NANO_PLATFORM_LINUX)
        #define MM_PLATFORM_LINUX
    #elif defined(NANO_PLATFORM_MACOS)
        #define MM_PLATFORM_MACOS
    #endif

	////////////////////////////////////////////////////////////////////////////////////
	// Typedefs
	////////////////////////////////////////////////////////////////////////////////////
	template<typename T, bool Destructable = false>
	using DeferredConstruct = Nano::Memory::DeferredConstruct<T, Destructable>;

	////////////////////////////////////////////////////////////////////////////////////
	// Types
	////////////////////////////////////////////////////////////////////////////////////
	enum class MessageType : uint8_t
	{
		Trace = 0, Info, Warn, Error, Fatal
	};

	////////////////////////////////////////////////////////////////////////////////////
	// Typedefs
	////////////////////////////////////////////////////////////////////////////////////
	template<typename T> using Vec2 = Obsidian::Maths::Vec2<T>;
	template<typename T> using Vec3 = Obsidian::Maths::Vec3<T>;
	template<typename T> using Vec4 = Obsidian::Maths::Vec4<T>;
	template<typename T> using Mat3 = Obsidian::Maths::Mat3<T>;
	template<typename T> using Mat4 = Obsidian::Maths::Mat4<T>;

	template<typename EventVariant>
	using EventHandler = Nano::Events::EventHandler<EventVariant>;

}