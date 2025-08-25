#pragma once

#include "Mario/Core/Core.hpp"

#include <Obsidian/Maths/Structs.hpp>

#include <cstdint>
#include <array>

namespace Mario
{

	////////////////////////////////////////////////////////////////////////////////////
	// LevelCamera
	////////////////////////////////////////////////////////////////////////////////////
	class LevelCamera
	{
	public:
		// Constructor & Destructor
		LevelCamera(const Obsidian::Maths::Vec2<float>& position, uint32_t width, uint32_t height);
		~LevelCamera();

		// Methods
		void Update();
		void Resize(uint32_t width, uint32_t height);

		// Getters
		inline const Obsidian::Maths::Mat4<float>& GetViewMatrix() const { return m_View; }
		inline const Obsidian::Maths::Mat4<float>& GetProjectionMatrix() const { return m_Projection; }

	private:
		const Obsidian::Maths::Vec2<float>& m_Position;
		
		Obsidian::Maths::Mat4<float> m_View = Obsidian::Maths::Mat4<float>(1.0f);
		Obsidian::Maths::Mat4<float> m_Projection = Obsidian::Maths::Mat4<float>(1.0f);
	};

}