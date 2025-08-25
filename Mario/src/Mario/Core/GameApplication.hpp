#pragma once

#include "Mario/Core/Core.hpp"

#include "Mario/Game/Game.hpp"

#include <Nano/Nano.hpp>

#include <Obsidian/Core/Window.hpp>
#include <Obsidian/Renderer/Device.hpp>
#include <Obsidian/Renderer/Swapchain.hpp>

#include <cstdint>
#include <queue>
#include <array>

namespace Mario
{

	class Texture;
	class Resources;
	class Renderer;
	class Game;
	class Level;

	////////////////////////////////////////////////////////////////////////////////////
	// Mario: Metal Game
	////////////////////////////////////////////////////////////////////////////////////
	class GameApplication
	{
	public:
		// Constructor & Destructor
		GameApplication();
		~GameApplication();

		// Methods
		void Run();

		// Static getter
		static GameApplication& Instance();

	private:
		// Private methods
		void OnMessage(MessageType type, const std::string& message);
		void OnEvent(Obsidian::Event& e);

		void DestroyQueue();

		// Private getters
		inline const Obsidian::Window& GetWindow() const { return m_Window.Get(); }
		inline Obsidian::Device& GetDevice() { return m_Device.Get(); }
		inline const Obsidian::Device& GetDevice() const { return m_Device.Get(); }
		inline const Obsidian::Swapchain& GetSwapchain() const { return m_Swapchain.Get(); }

	private:
		// Internals
		DeferredConstruct<Obsidian::Window> m_Window = {};
		DeferredConstruct<Obsidian::Device> m_Device = {};
		DeferredConstruct<Obsidian::Swapchain> m_Swapchain = {};

		std::array<DeferredConstruct<Obsidian::CommandListPool>, Obsidian::Information::FramesInFlight> m_CommandPools = { };
		std::array<DeferredConstruct<Obsidian::CommandList>, Obsidian::Information::FramesInFlight> m_CommandLists = { };

		std::queue<Obsidian::DeviceDestroyFn> m_DestroyQueue = { };

		DeferredConstruct<Game, true> m_Game = {};

		friend class Texture;
		friend class Resources;
		friend class Renderer;
		friend class Game;
		friend class Level;
	};

}