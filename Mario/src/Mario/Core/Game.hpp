#pragma once

#include "Mario/Core/Core.hpp"

#include "Mario/Renderer/Resources.hpp"
#include "Mario/Renderer/Renderer.hpp"

#include "Mario/Game/LevelCamera.hpp"

#include <Nano/Nano.hpp>

#include <Obsidian/Core/Window.hpp>
#include <Obsidian/Renderer/Device.hpp>
#include <Obsidian/Renderer/Swapchain.hpp>

#include <cstdint>
#include <queue>
#include <array>

namespace Mario
{

	class Sheet;
	class Resources;
	class Renderer;

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

		// Static getter
		static Game& Instance();

	private:
		// Private methods
		void OnMessage(MessageType type, const std::string& message);
		void OnEvent(Obsidian::Event& e);

		void DestroyQueue();

	private:
		// Internals
		DeferredConstruct<Obsidian::Window> m_Window = {};
		DeferredConstruct<Obsidian::Device> m_Device = {};
		DeferredConstruct<Obsidian::Swapchain> m_Swapchain = {};

		std::array<DeferredConstruct<Obsidian::CommandListPool>, Obsidian::Information::FramesInFlight> m_CommandPools = { };
		std::array<DeferredConstruct<Obsidian::CommandList>, Obsidian::Information::FramesInFlight> m_CommandLists = { };

		std::queue<Obsidian::DeviceDestroyFn> m_DestroyQueue = { };

		// Game
		DeferredConstruct<Resources, true> m_Resources = {};
		DeferredConstruct<Renderer, true> m_Renderer = {};

		// TODO: Abstract camera somehow
		Obsidian::Maths::Vec2<float> m_Position = { 0.0f, 0.0f };
		DeferredConstruct<LevelCamera, true> m_Camera = {};

		friend class Sheet;
		friend class Resources;
		friend class Renderer;
	};

}