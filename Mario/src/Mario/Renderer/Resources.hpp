#pragma once

#include "Mario/Core/Core.hpp"

#include "Mario/Renderer/Sheet.hpp"

#include <Obsidian/Renderer/Image.hpp>
#include <Obsidian/Maths/Structs.hpp>

#include <cstdint>
#include <string_view>

namespace Mario
{

	////////////////////////////////////////////////////////////////////////////////////
	// Resources
	////////////////////////////////////////////////////////////////////////////////////
	class Resources
	{
	public:
		struct UV
		{
		public:
			Obsidian::Maths::Vec2<float> TopLeft;
			Obsidian::Maths::Vec2<float> TopRight;
			Obsidian::Maths::Vec2<float> BottomLeft;
			Obsidian::Maths::Vec2<float> BottomRight;
		};

		struct Mario
		{
		public:
			inline static constexpr UV Standing = UV({ 0.0f, 0.0f }, { 16.0f / 480.0f , 0.0f }, { 0.0f, 16.0f / 620.0f }, { 16.0f / 620.0f, 16.0f / 620.0f });

		};
	public:
		// Constructor & Destructor
		Resources();
		~Resources();

		// Static getter
		static Resources& Instance();

	public:
		Sheet m_MarioLuigiSheet;
		Sheet m_EnemiesBossesSheet;
		Sheet m_ItemsObjectsSheet;
		Sheet m_TileSheet;
	};

}