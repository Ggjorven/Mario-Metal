#include "mmpch.h"
#include "Renderer.hpp"

#include "Mario/Core/Core.hpp"
#include "Mario/Core/Logging.hpp"
#include "Mario/Core/Game.hpp"

#include "Mario/Renderer/Resources.hpp"

namespace Mario
{

	////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////////////////////////////
	Renderer::Renderer(const Resources& resources)
		: m_Resources(resources)
	{
		auto& pool = Game::Instance().m_CommandPools[0].Get();
		auto list = pool.AllocateList(Obsidian::CommandListSpecification());

		list.Open();

		InitMain(list);
		InitBatch(list);

		list.Close();
		list.Submit(Obsidian::CommandListSubmitArgs());

		list.WaitTillComplete();
		pool.FreeList(list);
	}

	Renderer::~Renderer()
	{
		// TODO: Proper destroy
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	void Renderer::Begin()
	{

	}

	void Renderer::End()
	{

	}

	void Renderer::Flush()
	{

	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{

	}

	////////////////////////////////////////////////////////////////////////////////////
	// Private methods
	////////////////////////////////////////////////////////////////////////////////////
	void Renderer::InitMain(Obsidian::CommandList& list)
	{
		auto& device = Game::Instance().m_Device.Get();

		// White Texture
		m_WhiteTexture.Construct(device, Obsidian::ImageSpecification()
			.SetWidthAndHeight(1, 1)
			.SetImageDimension(Obsidian::ImageDimension::Image2D)
			.SetImageFormat(Obsidian::Format::RGBA8Unorm)
			.SetIsShaderResource(true)
			.SetDebugName("White Texture")
		);
		device.StartTracking(m_WhiteTexture.Get(), Obsidian::ImageSubresourceSpecification());

		auto stagingImage = device.CreateStagingImage(Obsidian::ImageSpecification()
			.SetWidthAndHeight(1, 1)
			.SetImageDimension(Obsidian::ImageDimension::Image2D)
			.SetImageFormat(Obsidian::Format::RGBA8Unorm)
			.SetIsShaderResource(true)
			.SetDebugName("White Staging Texture"), 
			Obsidian::CpuAccessMode::Write
		);
		device.StartTracking(stagingImage);

		uint32_t white = 0xFFFFFFFF;
		device.WriteImage(stagingImage, Obsidian::ImageSliceSpecification(), &white, sizeof(white));

		list.CopyImage(m_WhiteTexture.Get(), Obsidian::ImageSliceSpecification(), stagingImage, Obsidian::ImageSliceSpecification());

		// Camera buffer
		m_CameraBuffer.Construct(device, Obsidian::BufferSpecification()
			.SetSize(sizeof(RendererCamera))
			.SetIsUniformBuffer(true)
			.SetCPUAccess(Obsidian::CpuAccessMode::Write)
			.SetPermanentState(Obsidian::ResourceState::ShaderResource)
		);

		RendererCamera camera;
		device.WriteBuffer(m_CameraBuffer.Get(), &camera, sizeof(camera));

		device.StopTracking(stagingImage);
		device.DestroyStagingImage(stagingImage);
	}

	void Renderer::InitBatch(Obsidian::CommandList& list)
	{
		auto& device = Game::Instance().m_Device.Get();

		device.DestroyBuffer(m_CameraBuffer.Get());
		device.DestroyImage(m_WhiteTexture.Get());
	}

}