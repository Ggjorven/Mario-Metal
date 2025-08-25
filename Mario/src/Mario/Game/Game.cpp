#include "mmpch.h"
#include "Game.hpp"

#include "Mario/Core/Core.hpp"
#include "Mario/Core/Logging.hpp"
#include "Mario/Core/GameApplication.hpp"

namespace Mario
{

	////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////////////////////////////
	Game::Game(uint32_t width, uint32_t height)
		: m_Camera({}, width, height)
	{
	}

	Game::~Game()
	{
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	void Game::OnUpdate(double deltaTime)
	{
		(void)deltaTime;

		m_Camera.Update();
	}

	void Game::OnRender(Obsidian::CommandList& list)
	{
		m_Renderer.Begin(m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix());

		m_Renderer.DrawQuad({ 0.0f, 0.0f, 0.0f }, { 64.0f, 64.0f }, UVMaps::Mario::Standing, Renderer::TextureID::MarioLuigi);

		m_Renderer.End(list, { 0.0f, 0.0f, 0.0f, 1.0f });
	}

	void Game::OnEvent(Obsidian::Event& e)
	{
		EventHandler handler(e);

		handler.Handle<Obsidian::WindowResizeEvent>([this](Obsidian::WindowResizeEvent& wre)
		{
			m_Renderer.Resize(wre.GetWidth(), wre.GetHeight());
			m_Camera.Resize(wre.GetWidth(), wre.GetHeight());
		});
	}

}