#pragma once

#include "Mario/Core/Core.hpp"

#include "Mario/Game/LevelCamera.hpp"

#include "Mario/Renderer/Renderer.hpp"

#include <Obsidian/Core/Events.hpp>
#include <Obsidian/Renderer/CommandList.hpp>

#include <cstdint>

namespace Mario
{

	////////////////////////////////////////////////////////////////////////////////////
	// Game
	////////////////////////////////////////////////////////////////////////////////////
	class Game
	{
	public:
		// Constructor & Destructor
		Game(uint32_t width, uint32_t height);
		~Game();

		// Methods
		void OnUpdate(double deltaTime);
		void OnRender(Obsidian::CommandList& list);
		void OnEvent(Obsidian::Event& e);

	private:
		LevelCamera m_Camera;
		Renderer m_Renderer;
	};

}