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
	LevelCamera::LevelCamera(const Obsidian::Maths::Vec2<float>& position, uint32_t width, uint32_t height)
		: m_Position(position)
	{
		Resize(width, height);
		Update();
	}

	LevelCamera::~LevelCamera()
	{
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	void LevelCamera::Resize(uint32_t width, uint32_t height)
	{
		//m_Projection = Obsidian::Maths::Orthographic(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
		m_Projection = Obsidian::Maths::Orthographic(Obsidian::Maths::AspectRatio(width, height));
		m_Projection = Obsidian::Maths::ApplyProjectionCorrection(m_Projection);
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Private methods
	////////////////////////////////////////////////////////////////////////////////////
	void LevelCamera::Update()
	{
		m_View = Obsidian::Maths::Translate(Obsidian::Maths::Mat4<float>(1.0f), { m_Position.x, m_Position.y, 0.0f });
	}

}