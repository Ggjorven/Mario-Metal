#pragma once

#include "Mario/Core/Core.hpp"

#include <Obsidian/Renderer/Image.hpp>

#include <cstdint>
#include <filesystem>

namespace Mario
{

	class Renderer;

	////////////////////////////////////////////////////////////////////////////////////
	// Texture
	////////////////////////////////////////////////////////////////////////////////////
	class Texture
	{
	public:
		// Constructor & Destructor
		Texture(const std::filesystem::path& path);
		~Texture();

	private:
		// Private methods
		uint8_t* LoadFromPath(const std::filesystem::path& path, uint32_t& outWidth, uint32_t& outHeight) const;

	private:
		DeferredConstruct<Obsidian::Image> m_Image = {};

		friend class Renderer;
	};

}