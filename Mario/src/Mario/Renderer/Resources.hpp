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
	// UV
	////////////////////////////////////////////////////////////////////////////////////
	struct UV
	{
	public:
		Obsidian::Maths::Vec2<float> TopLeft = { 0.0f, 0.0f };
		Obsidian::Maths::Vec2<float> TopRight = { 1.0f, 0.0f };
		Obsidian::Maths::Vec2<float> BottomLeft = { 0.0f, 1.0f };
		Obsidian::Maths::Vec2<float> BottomRight = { 1.0f, 1.0f };

	public:
		// Methods
		inline static constexpr UV Flip(const UV& uv)
		{
			UV result;

			result.TopLeft = uv.TopRight;
			result.TopRight = uv.TopLeft;

			result.BottomLeft = uv.BottomRight;
			result.BottomRight = uv.BottomLeft;

			return result;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////
	// Resources
	////////////////////////////////////////////////////////////////////////////////////
	class Resources
	{
	public:
		struct Mario
		{
		public:
			inline static constexpr UV Standing = UV({ 0.0f, 0.0f }, { 16.0f / 480.0f , 0.0f }, { 0.0f, 16.0f / 620.0f }, { 16.0f / 480.0f, 16.0f / 620.0f });

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