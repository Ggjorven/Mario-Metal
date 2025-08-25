#pragma once

#include "Mario/Core/Core.hpp"

#include <Obsidian/Maths/Structs.hpp>

#include <cstdint>

namespace Mario
{

	////////////////////////////////////////////////////////////////////////////////////
	// LevelCamera
	////////////////////////////////////////////////////////////////////////////////////
	class LevelCamera
	{
	public:
		// Constructor & Destructor
		LevelCamera(uint32_t width, uint32_t height);
		~LevelCamera();

		// Methods
		void Update(const Vec2<float>& position);
		void Resize(uint32_t width, uint32_t height);

		// Getters
		inline const Mat4<float>& GetViewMatrix() const { return m_View; }
		inline const Mat4<float>& GetProjectionMatrix() const { return m_Projection; }

	private:
		Mat4<float> m_View = Mat4<float>(1.0f);
		Mat4<float> m_Projection = Mat4<float>(1.0f);
	};

}