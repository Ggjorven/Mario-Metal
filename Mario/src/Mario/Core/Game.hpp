#pragma once

#include "Mario/Core/Core.hpp"

#include <Nano/Nano.hpp>

#include <Obsidian/Core/Window.hpp>
#include <Obsidian/Renderer/Device.hpp>

#include <cstdint>
#include <queue>

namespace Mario
{

	////////////////////////////////////////////////////////////////////////////////////
	// Mario: Metal Game
	////////////////////////////////////////////////////////////////////////////////////
	class Game
	{
	public:
		// Constructor & Destructor
		Game();
		~Game();

		// Methods
		void Run();

	private:
		// Private methods
		void OnMessage(MessageType type, const std::string& message);
		void OnEvent(Obsidian::Event& e);

	private:
		DeferredConstruct<Obsidian::Window> m_Window = {};
		DeferredConstruct<Obsidian::Device> m_Device = {};

		std::queue<Obsidian::DeviceDestroyFn> m_DestroyQueue = { };
	};

}