#include "mmpch.h"
#include "Texture.hpp"

#include "Mario/Core/Core.hpp"
#include "Mario/Core/Logging.hpp"
#include "Mario/Core/Settings.hpp"

#include "Mario/Core/GameApplication.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <format>

namespace Mario
{

    ////////////////////////////////////////////////////////////////////////////////////
    // Constructor & Destructor
    ////////////////////////////////////////////////////////////////////////////////////
    Texture::Texture(const std::filesystem::path& path)
    {
        uint32_t width, height;
        uint8_t* pixels = LoadFromPath(path, width, height);

        auto& game = GameApplication::Instance();

        m_Image.Construct(game.GetDevice(), Obsidian::ImageSpecification()
            .SetWidthAndHeight(width, height)
            .SetImageDimension(Obsidian::ImageDimension::Image2D)
            .SetImageFormat(Obsidian::Format::RGBA8Unorm)
            .SetIsShaderResource(true)
            .SetMipLevels(1)
            .SetDebugName(std::format("Sheet from: {0}", path.string()))
        );

        auto& device = game.m_Device.Get();
        device.StartTracking(m_Image.Get(), Obsidian::ImageSubresourceSpecification());

        auto stagingImage = device.CreateStagingImage(m_Image->GetSpecification(), Obsidian::CpuAccessMode::Write);
        device.StartTracking(stagingImage);

        // TODO: Reuse commandbuffers
        {
            auto list = game.m_CommandPools[0]->AllocateList(Obsidian::CommandListSpecification());
            list.Open();

            device.WriteImage(stagingImage, Obsidian::ImageSliceSpecification(), pixels, static_cast<size_t>(width) * height * 4);
            list.CopyImage(m_Image.Get(), Obsidian::ImageSliceSpecification(), stagingImage, Obsidian::ImageSliceSpecification());

            list.Close();
            list.Submit(Obsidian::CommandListSubmitArgs());
            list.WaitTillComplete();

            game.m_CommandPools[0]->FreeList(list);
        }

        device.DestroyStagingImage(stagingImage);
        device.StopTracking(stagingImage);

        stbi_image_free(static_cast<void*>(pixels));
    }

    Texture::~Texture()
    {
        GameApplication::Instance().GetDevice().DestroyImage(m_Image.Get());
    }

    ////////////////////////////////////////////////////////////////////////////////////
    // Private methods
    ////////////////////////////////////////////////////////////////////////////////////
    uint8_t* Texture::LoadFromPath(const std::filesystem::path& path, uint32_t& outWidth, uint32_t& outHeight) const
    {
        int width, height, channels;
        stbi_uc* pixels = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha); // Padds the image with alpha if it has none

        //MM_ASSERT((channels == static_cast<int>(STBI_rgb_alpha)), "Channels must be equal to requested channels, 4.");

        outWidth = static_cast<uint32_t>(width);
        outHeight = static_cast<uint32_t>(height);

        return pixels;
    }

}