#pragma once

#include <cstdint>
#include <array>
#include <filesystem>

#include <Obsidian/Maths/Structs.hpp>

// TODO: Make part of this a user file, json or yaml

namespace Mario::Settings
{

	////////////////////////////////////////////////////////////////////////////////////
	// Window settings
	////////////////////////////////////////////////////////////////////////////////////
	inline constexpr const uint32_t WindowWidth = 1280;
	inline constexpr const uint32_t WindowHeight = 720;

	// Note: Relative to ProjectDirectory
	inline constexpr const std::string_view WindowIcon = ""; // FUTURE TODO: ...

	////////////////////////////////////////////////////////////////////////////////////
	// Sheet settings
	////////////////////////////////////////////////////////////////////////////////////
	inline constexpr const std::string_view MarioLuigiSheet = "Resources/Images/Mario-Luigi-Sheet-1.png";
	inline constexpr const std::string_view ItemsObjectsSheet = "Resources/Images/items-objects-sheet.png";
	inline constexpr const std::string_view EnemiesBossesSheet = "Resources/Images/enemies-bosses-sheet.png";
	inline constexpr const std::string_view TileSheet = "Resources/Images/tile-sheet.png";

}