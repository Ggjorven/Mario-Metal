#pragma once

#include "Mario/Core/Core.hpp"

#include "Mario/Renderer/Renderer.hpp"

#include <Nano/Nano.hpp>

#include <cstdint>
#include <tuple>
#include <filesystem>

namespace Mario
{

	////////////////////////////////////////////////////////////////////////////////////
	// Components
	////////////////////////////////////////////////////////////////////////////////////
	struct TransformComponent
	{
	public:
		Vec2<float> Position;
		Vec2<float> Size;
	};

	struct SpriteComponent
	{
	public:
		const UV& UVs;
		Renderer::TextureID TextureID;
	};

	using EntityID = uint32_t;
	using Components = std::tuple<TransformComponent, SpriteComponent>;

	////////////////////////////////////////////////////////////////////////////////////
	// Level
	////////////////////////////////////////////////////////////////////////////////////
	class Level // TODO: Proper serialization
	{
	public:
		// Constructor & Destructor
		Level() = default;
		Level(const std::filesystem::path& path);
		~Level();

		// Methods
		void Update(double deltaTime);
		void Render(Renderer& renderer);
		
		void LoadFromFile(const std::filesystem::path& path);

		// Getters
		inline EntityID GetControlledCharacterID() const { return m_ControlledCharacterID; }
		inline EntityID GetMarioID() const { return m_MarioID; }
		inline EntityID GetLuigiID() const { return m_LuigiID; }

	private:
		Nano::ECS::Registry<EntityID, Components> m_Registry;

		// Main characters
		EntityID m_MarioID = 0;
		EntityID m_LuigiID = 1;

		EntityID m_ControlledCharacterID = m_MarioID; // The character being controlled
		EntityID m_CurrentID = m_LuigiID + 1;
	};

}