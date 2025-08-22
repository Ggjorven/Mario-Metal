#pragma once

#include "Mario/Core/Core.hpp"

#include <Obsidian/Renderer/Image.hpp>

#include <cstdint>
#include <filesystem>

namespace Mario
{

	class Renderer;

	////////////////////////////////////////////////////////////////////////////////////
	// Sheet
	////////////////////////////////////////////////////////////////////////////////////
	class Sheet
	{
	public:
		// Constructor & Destructor
		Sheet(const std::filesystem::path& path);
		~Sheet();

	private:
		// Private methods
		uint8_t* LoadFromPath(const std::filesystem::path& path, uint32_t& outWidth, uint32_t& outHeight) const;

	private:
		DeferredConstruct<Obsidian::Image> m_Image = {};

		friend class Renderer;
	};

}