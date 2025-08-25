#include "mmpch.h"
#include "LevelCamera.hpp"

#include "Mario/Core/Core.hpp"
#include "Mario/Core/Logging.hpp"

#include <Obsidian/Maths/Functions.hpp>

namespace Mario
{

	////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////////////////////////////
	LevelCamera::LevelCamera(uint32_t width, uint32_t height)
	{
		Resize(width, height);
		m_View = Obsidian::Maths::Translate(Mat4<float>(1.0f), { 0.0f, 0.0f, 0.0f });
	}

	LevelCamera::~LevelCamera()
	{
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	void LevelCamera::Update(const Vec2<float>& position)
	{
		m_View = Obsidian::Maths::Translate(Mat4<float>(1.0f), { position.x, position.y, 0.0f });
	}

	void LevelCamera::Resize(uint32_t width, uint32_t height)
	{
		// Note: Has projection correction built in.
		m_Projection = Obsidian::Maths::Orthographic(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
	}

}