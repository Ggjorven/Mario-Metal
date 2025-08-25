#include "mmpch.h"
#include "Level.hpp"

#include "Mario/Core/Core.hpp"
#include "Mario/Core/Logging.hpp"
#include "Mario/Core/Settings.hpp"
#include "Mario/Core/GameApplication.hpp"

#include <Obsidian/Maths/Functions.hpp>

namespace Mario
{

	////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////////////////////////////
	Level::Level(const std::filesystem::path& path)
	{
		LoadFromFile(path);
	}

	Level::~Level()
	{
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	void Level::Update(double deltaTime)
	{
		float dt = static_cast<float>(deltaTime);
		auto& input = GameApplication::Instance().GetWindow().GetInput();

		TransformComponent& transform = m_Registry.GetComponent<TransformComponent>(m_ControlledCharacterID);
		SpriteComponent& sprite = m_Registry.GetComponent<SpriteComponent>(m_ControlledCharacterID);

		if (input.IsKeyPressed(Obsidian::Key::W))
		{
			transform.Position.y += 100.0f * dt;
		}
		if (input.IsKeyPressed(Obsidian::Key::S))
		{
			transform.Position.y -= 100.0f * dt;
		}
		if (input.IsKeyPressed(Obsidian::Key::A))
		{
			transform.Position.x -= 100.0f * dt;
		}
		if (input.IsKeyPressed(Obsidian::Key::D))
		{
			transform.Position.x += 100.0f * dt;
		}
	}

	void Level::Render(Renderer& renderer)
	{
		for (const auto& [_, transform, sprite] : m_Registry.View<TransformComponent, SpriteComponent>())
		{
			renderer.DrawQuad({ transform.Position.x, transform.Position.y, 0.0f }, transform.Size, sprite.UVs, sprite.TextureID);
		}
	}

	void Level::LoadFromFile(const std::filesystem::path& path)
	{
		// TODO: Actually load from file
		
		// Mario
		{
			m_Registry.AddComponent<TransformComponent>(m_MarioID, { { 0.0f, 0.0f }, { 16.0f * Settings::Scale, 16.0f * Settings::Scale } });
			m_Registry.AddComponent<SpriteComponent>(m_MarioID, { UVMaps::Mario::Standing, Renderer::TextureID::MarioLuigi });
		}
	}

}