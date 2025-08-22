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
	inline constexpr const std::string_view WindowIcon = "";

	////////////////////////////////////////////////////////////////////////////////////
	// Sheet settings
	////////////////////////////////////////////////////////////////////////////////////
	inline constexpr const auto SheetTransparency = std::to_array<std::array<uint8_t, 3>>({
		std::array<uint8_t, 3>({ 0, 41, 140 }),
		std::array<uint8_t, 3>({ 146, 144, 255 }),
		std::array<uint8_t, 3>({ 108, 106, 255 }),
		std::array<uint8_t, 3>({ 92, 148, 252 }),
		std::array<uint8_t, 3>({ 0, 0, 168 }),
	});

	inline constexpr const std::string_view MarioLuigiSheet = "Resources/Images/Mario-Luigi-Sheet-1.png";
	inline constexpr const std::string_view ItemsObjectsSheet = "Resources/Images/items-objects-sheet.png";
	inline constexpr const std::string_view EnemiesBossesSheet = "Resources/Images/enemies-bosses-sheet.png";
	inline constexpr const std::string_view TileSheet = "Resources/Images/tile-sheet.png";

}