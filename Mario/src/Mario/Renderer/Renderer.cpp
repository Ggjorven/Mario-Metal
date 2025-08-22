#include "mmpch.h"
#include "Renderer.hpp"

#include "Mario/Core/Core.hpp"
#include "Mario/Core/Logging.hpp"
#include "Mario/Core/Game.hpp"

#include "Mario/Renderer/Resources.hpp"

#include <Obsidian/Maths/Functions.hpp>

namespace
{

	static constexpr std::string_view g_VertexShader = R"(
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Colour;
layout(location = 3) in uint a_TextureID;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec2 v_TexCoord;
layout(location = 2) out vec4 v_Colour;
layout(location = 3) flat out uint v_TextureID;

layout(std140, set = 0, binding = 0) uniform CameraSettings
{
    mat4 View;
    mat4 Projection;
} u_Camera;

void main()
{
    v_Position = a_Position;
    v_TexCoord = a_TexCoord;
    v_Colour = a_Colour;
    v_TextureID = a_TextureID;

    gl_Position = u_Camera.Projection * u_Camera.View * vec4(a_Position, 1.0);
}
	)";

	static constexpr std::string_view g_FragmentShader = R"(
#version 460 core
//#extension GL_KHR_vulkan_glsl : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 o_Colour;

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec2 v_TexCoord;
layout(location = 2) in vec4 v_Colour;

layout(location = 3) flat in uint v_TextureID;

layout (set = 0, binding = 1) uniform texture2D u_Textures[];
layout (set = 0, binding = 2) uniform sampler u_Sampler;

void main()
{
	// TODO: Make sure this dynamic indexing is fine on all platforms
    o_Colour = v_Colour * texture(sampler2D(u_Textures[v_TextureID], u_Sampler), v_TexCoord);
}
	)";

}

namespace Mario
{

	////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////////////////////////////
	Renderer::Renderer(Resources& resources)
		: m_Resources(resources)
	{
		auto& pool = Game::Instance().m_CommandPools[0].Get();
		auto list = pool.AllocateList(Obsidian::CommandListSpecification());

		list.Open();

		InitMain(list);
		InitBatch(list);
		InitMario(list);

		list.Close();
		list.Submit(Obsidian::CommandListSubmitArgs());

		list.WaitTillComplete();
		pool.FreeList(list);
	}

	Renderer::~Renderer()
	{
		auto& device = Game::Instance().m_Device.Get();

		device.DestroyInputLayout(m_Batch.InputLayout.Get());
		device.DestroyBindingLayout(m_Batch.BindingLayoutSet0.Get());
		device.DestroyGraphicsPipeline(m_Batch.Pipeline.Get());

		device.FreeBindingSetPool(m_Batch.Set0Pool.Get());

		device.DestroyRenderpass(m_Batch.Renderpass.Get());

		device.DestroyBuffer(m_Batch.IndexBuffer.Get());
		device.DestroyBuffer(m_Batch.VertexBuffer.Get());

		device.DestroyBuffer(m_CameraBuffer.Get());
		device.DestroySampler(m_TextureSampler.Get());
		device.DestroyImage(m_WhiteTexture.Get());
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	void Renderer::Begin()
	{
		m_Batch.CPUBuffer.clear();

		// Temporary check
		{
			//DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, Resources::UV(), TextureID::MarioLuigi);
			DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, Resources::Mario::Standing, TextureID::MarioLuigi);
		}
	}

	void Renderer::End()
	{
		if (!m_Batch.CPUBuffer.empty())
		{
			auto& device = Game::Instance().m_Device.Get();
			device.WriteBuffer(m_Batch.VertexBuffer.Get(), static_cast<const void*>(m_Batch.CPUBuffer.data()), (m_Batch.CPUBuffer.size() * sizeof(RendererVertex)));
		}
	}

	void Renderer::Flush(Obsidian::CommandList& list)
	{
		auto& window = Game::Instance().m_Window.Get();

		list.StartRenderpass(Obsidian::RenderpassStartArgs()
			.SetRenderpass(m_Batch.Renderpass.Get())

			.SetViewport(Obsidian::Viewport(static_cast<float>(window.GetSize().x), static_cast<float>(window.GetSize().y)))
			.SetScissor(Obsidian::ScissorRect(Obsidian::Viewport(static_cast<float>(window.GetSize().x), static_cast<float>(window.GetSize().y))))

			.SetColourClear({ 0.0f, 0.0f, 0.0f, 1.0f }) // TODO: Based on level colour?
		);

		list.BindPipeline(m_Batch.Pipeline.Get());

		list.BindVertexBuffer(m_Batch.VertexBuffer.Get());
		list.BindIndexBuffer(m_Batch.IndexBuffer.Get());

		list.BindBindingSet(m_Batch.Set0.Get());

		list.DrawIndexed(Obsidian::DrawArguments()
			.SetVertexCount(static_cast<uint32_t>(((m_Batch.CPUBuffer.size() / 4ull) * 6ull)))
			.SetInstanceCount(1)
		);

		list.EndRenderpass(Obsidian::RenderpassEndArgs()
			.SetRenderpass(m_Batch.Renderpass.Get())
		);
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		(void)width; (void)height; // Note: Useless I know, but maybe useful in the future
		m_Batch.Renderpass->ResizeFramebuffers();
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Init methods
	////////////////////////////////////////////////////////////////////////////////////
	void Renderer::InitMain(Obsidian::CommandList& list)
	{
		auto& game = Game::Instance();
		auto& device = game.m_Device.Get();

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

		// Sampler
		m_TextureSampler.Construct(device, Obsidian::SamplerSpecification()
			.SetAllFilters(Obsidian::FilterMode::Nearest)
			.SetAllAddressModes(Obsidian::SamplerAddressMode::Clamp)

			.SetDebugName("Texture sampler")
		);

		// Camera buffer
		m_CameraBuffer.Construct(device, Obsidian::BufferSpecification()
			.SetSize(sizeof(RendererCamera))
			.SetIsUniformBuffer(true)
			.SetCPUAccess(Obsidian::CpuAccessMode::Write)
			.SetPermanentState(Obsidian::ResourceState::ShaderResource)
		);

		RendererCamera camera;
		camera.Projection = Obsidian::Maths::Orthographic(Obsidian::Maths::AspectRatio(game.m_Window->GetSize().x, game.m_Window->GetSize().y));
		camera.Projection = Obsidian::Maths::ApplyProjectionCorrection(camera.Projection);

		device.WriteBuffer(m_CameraBuffer.Get(), &camera, sizeof(camera));

		device.StopTracking(stagingImage);
		device.DestroyStagingImage(stagingImage);
	}

	void Renderer::InitBatch(Obsidian::CommandList& list)
	{
		auto& game = Game::Instance();
		auto& device = game.m_Device.Get();

		// Renderpass
		{
			m_Batch.Renderpass.Construct(device, Obsidian::RenderpassSpecification()
				.SetBindpoint(Obsidian::PipelineBindpoint::Graphics)

				.SetColourImageSpecification(game.m_Swapchain->GetImage(0).GetSpecification())
				.SetColourLoadOperation(Obsidian::LoadOperation::Clear)
				.SetColourStoreOperation(Obsidian::StoreOperation::Store)
				.SetColourStartState(Obsidian::ResourceState::Present)
				.SetColourRenderingState(Obsidian::ResourceState::RenderTarget)
				.SetColourEndState(Obsidian::ResourceState::Present)

				.SetDebugName("Batch renderpass")
			);

			for (uint8_t i = 0; i < Obsidian::Information::FramesInFlight; i++)
			{
				m_Batch.Renderpass->CreateFramebuffer(Obsidian::FramebufferSpecification()
					.SetColourAttachment(Obsidian::FramebufferAttachment()
						.SetImage(game.m_Swapchain->GetImage(i))
					)

					.SetDebugName(std::format("Framebuffer ({0})", static_cast<size_t>(i)))
				);
			}
		}

		// Shaders

		Obsidian::ShaderCompiler compiler;
		auto vertexSpv = compiler.CompileToSPIRV(Obsidian::ShaderStage::Vertex, std::string(g_VertexShader), "main", Obsidian::ShadingLanguage::GLSL);
		auto fragmentSpv = compiler.CompileToSPIRV(Obsidian::ShaderStage::Fragment, std::string(g_FragmentShader), "main", Obsidian::ShadingLanguage::GLSL);

		Obsidian::Shader vertexShader = device.CreateShader(Obsidian::ShaderSpecification()
			.SetShaderStage(Obsidian::ShaderStage::Vertex)
			.SetSPIRV(vertexSpv)
			.SetMainName("main")

			.SetDebugName("Vertex Shader")
		);

		Obsidian::Shader fragmentShader = device.CreateShader(Obsidian::ShaderSpecification()
			.SetShaderStage(Obsidian::ShaderStage::Fragment)
			.SetSPIRV(fragmentSpv)
			.SetMainName("main")

			.SetDebugName("Fragment Shader")
		);

		// BindingSets
		{
			m_Batch.BindingLayoutSet0.Construct(device, Obsidian::BindlessLayoutSpecification()
				.SetSetIndex(0)

				.AddItem(Obsidian::BindingLayoutItem()
					.SetSlot(0)
					.SetType(Obsidian::ResourceType::UniformBuffer)
					.SetVisibility(Obsidian::ShaderStage::Vertex)
					.SetDebugName("u_Camera")
				)

				.AddItem(Obsidian::BindingLayoutItem()
					.SetSlot(1)
					.SetSize(MaxTextures)
					.SetType(Obsidian::ResourceType::Image)
					.SetVisibility(Obsidian::ShaderStage::Fragment)
					.SetDebugName("u_Textures")
				)
				.AddItem(Obsidian::BindingLayoutItem()
					.SetSlot(2)
					.SetType(Obsidian::ResourceType::Sampler)
					.SetVisibility(Obsidian::ShaderStage::Fragment)
					.SetDebugName("u_Sampler")
				)

				.SetDebugName("BindingSetLayout for set 0")
			);

			m_Batch.Set0Pool.Construct(device, Obsidian::BindingSetPoolSpecification()
				.SetSetAmount(1)
				.SetLayout(m_Batch.BindingLayoutSet0)

				.SetDebugName("Set 0 pool")
			);

			m_Batch.Set0.Construct(m_Batch.Set0Pool.Get(), Obsidian::BindingSetSpecification()
				.SetDebugName("Set 0")
			);
		}

		// Pipeline
		{
			m_Batch.InputLayout.Construct(device, std::to_array<Obsidian::VertexAttributeSpecification>({
				Obsidian::VertexAttributeSpecification()
					.SetLocation(0)
					.SetFormat(Obsidian::Format::RGB32Float)
					
					.SetSize(Obsidian::VertexAttributeSpecification::AutoSize)
					.SetOffset(Obsidian::VertexAttributeSpecification::AutoOffset)

					.SetDebugName("a_Position"),

				Obsidian::VertexAttributeSpecification()
					.SetLocation(1)
					.SetFormat(Obsidian::Format::RG32Float)

					.SetSize(Obsidian::VertexAttributeSpecification::AutoSize)
					.SetOffset(Obsidian::VertexAttributeSpecification::AutoOffset)

					.SetDebugName("a_TexCoord"),

				Obsidian::VertexAttributeSpecification()
					.SetLocation(2)
					.SetFormat(Obsidian::Format::RGBA32Float)

					.SetSize(Obsidian::VertexAttributeSpecification::AutoSize)
					.SetOffset(Obsidian::VertexAttributeSpecification::AutoOffset)

					.SetDebugName("a_Colour"),

				Obsidian::VertexAttributeSpecification()
					.SetLocation(3)
					.SetFormat(Obsidian::Format::R32UInt)

					.SetSize(Obsidian::VertexAttributeSpecification::AutoSize)
					.SetOffset(Obsidian::VertexAttributeSpecification::AutoOffset)

					.SetDebugName("a_TextureID"),
			}));

			m_Batch.Pipeline.Construct(device, Obsidian::GraphicsPipelineSpecification()
				.SetDebugName("Renderer pipeline")

				.SetInputLayout(m_Batch.InputLayout.Get())

				.SetVertexShader(vertexShader)
				.SetFragmentShader(fragmentShader)

				.SetRenderState(Obsidian::RenderState()
					.SetRasterState(Obsidian::RasterState()
						.SetFillMode(Obsidian::RasterFillMode::Fill)
						.SetCullingMode(Obsidian::RasterCullingMode::None)
						.SetFrontCounterClockwise(true)
						.SetDepthBias(0)
						.SetDepthBiasClamp(0.0f)
					)
					.SetBlendState(Obsidian::BlendState()
						.SetRenderTarget(Obsidian::BlendState::RenderTarget()
							.SetBlendEnable(true)
							.SetSrcBlend(Obsidian::BlendFactor::SrcAlpha)
							.SetDstBlend(Obsidian::BlendFactor::OneMinusSrcAlpha)
							.SetBlendOperation(Obsidian::BlendOperation::Add)
							.SetSrcBlendAlpha(Obsidian::BlendFactor::One)
							.SetDstBlendAlpha(Obsidian::BlendFactor::OneMinusSrcAlpha)
							.SetBlendOpAlpha(Obsidian::BlendOperation::Add)
							.SetColourWriteMask(Obsidian::ColourMask::All)
						)
						.SetAlphaToCoverageEnable(false)
					)
					.SetDepthStencilState(Obsidian::DepthStencilState()
						.SetDepthTestEnable(false)	// Note: Enable for depth
						.SetDepthWriteEnable(false)	// Note: Enable for depth
						.SetDepthFunc(Obsidian::ComparisonFunc::Less)
						.SetStencilEnable(false)
					)
				)

				.SetRenderpass(m_Batch.Renderpass.Get())
				.AddBindingLayout(m_Batch.BindingLayoutSet0.Get())

				.SetDebugName("GraphicsPipeline")
			);
		}

		device.DestroyShader(vertexShader);
		device.DestroyShader(fragmentShader);

		// Buffers
		{
			constexpr std::array<uint32_t, (MaxQuads * 6)> indices = []() 
			{
				std::array<uint32_t, (MaxQuads * 6)> array = { };

				for (uint32_t i = 0, offset = 0; i < MaxQuads; i += 6, offset += 4) 
				{
					array[static_cast<size_t>(i) + 0] = offset + 0;
					array[static_cast<size_t>(i) + 1] = offset + 1;
					array[static_cast<size_t>(i) + 2] = offset + 2;

					array[static_cast<size_t>(i) + 3] = offset + 2;
					array[static_cast<size_t>(i) + 4] = offset + 3;
					array[static_cast<size_t>(i) + 5] = offset + 0;
				}

				return array;
			}();

			Obsidian::Buffer stagingBuffer = device.CreateBuffer(Obsidian::BufferSpecification()
				.SetSize(sizeof(indices))
				.SetCPUAccess(Obsidian::CpuAccessMode::Write)
			);
			device.StartTracking(stagingBuffer);

			m_Batch.IndexBuffer.Construct(device, Obsidian::BufferSpecification()
				.SetFormat(Obsidian::Format::R32UInt)
				.SetSize(sizeof(indices))
				.SetIsIndexBuffer(true)
				.SetDebugName("IndexBuffer")
			);
			device.StartTracking(m_Batch.IndexBuffer.Get(), Obsidian::ResourceState::IndexBuffer);

			device.WriteBuffer(stagingBuffer, static_cast<const void*>(indices.data()), sizeof(indices));
			list.CopyBuffer(m_Batch.IndexBuffer.Get(), stagingBuffer, sizeof(indices));

			m_Batch.VertexBuffer.Construct(device, Obsidian::BufferSpecification()
				.SetSize(MaxQuads * sizeof(RendererVertex))
				.SetIsVertexBuffer(true)
				.SetCPUAccess(Obsidian::CpuAccessMode::Write)
				.SetDebugName("Vertexbuffer")
			);
			device.StartTracking(m_Batch.VertexBuffer.Get(), Obsidian::ResourceState::VertexBuffer);

			device.StopTracking(stagingBuffer);
			device.DestroyBuffer(stagingBuffer);
		}
	}

	void Renderer::InitMario(Obsidian::CommandList& list)
	{
		(void)list;

		m_Batch.Set0->SetItem(0, m_CameraBuffer.Get()); // Camera

		m_Batch.Set0->SetItem(1, m_WhiteTexture.Get(), Obsidian::ImageSubresourceSpecification(), static_cast<uint32_t>(TextureID::White));
		m_Batch.Set0->SetItem(1, m_Resources.m_MarioLuigiSheet.m_Image.Get(), Obsidian::ImageSubresourceSpecification(), static_cast<uint32_t>(TextureID::MarioLuigi));
		m_Batch.Set0->SetItem(1, m_Resources.m_EnemiesBossesSheet.m_Image.Get(), Obsidian::ImageSubresourceSpecification(), static_cast<uint32_t>(TextureID::EnemiesBosses));
		m_Batch.Set0->SetItem(1, m_Resources.m_ItemsObjectsSheet.m_Image.Get(), Obsidian::ImageSubresourceSpecification(), static_cast<uint32_t>(TextureID::ItemsObjects));
		m_Batch.Set0->SetItem(1, m_Resources.m_TileSheet.m_Image.Get(), Obsidian::ImageSubresourceSpecification(), static_cast<uint32_t>(TextureID::Tiles));

		m_Batch.Set0->SetItem(2, m_TextureSampler.Get()); // Sampler

	}

	////////////////////////////////////////////////////////////////////////////////////
	// Private methods
	////////////////////////////////////////////////////////////////////////////////////
	void Renderer::DrawQuad(const Obsidian::Maths::Vec3<float>& position, const Obsidian::Maths::Vec2<float>& size, const Resources::UV& uv, TextureID textureID)
	{
		// Note: I can't figure out why the UVs in this project are different
		// FUTURE TODO: ...

		//constexpr const Obsidian::Maths::Vec2<float> uv0(1.0f, 0.0f);
		//constexpr const Obsidian::Maths::Vec2<float> uv1(0.0f, 0.0f);
		//constexpr const Obsidian::Maths::Vec2<float> uv2(0.0f, 1.0f);
		//constexpr const Obsidian::Maths::Vec2<float> uv3(1.0f, 1.0f);

		//constexpr const Obsidian::Maths::Vec2<float> uv0(1.0f, 1.0f); // top-right
		//constexpr const Obsidian::Maths::Vec2<float> uv1(0.0f, 1.0f); // top-left
		//constexpr const Obsidian::Maths::Vec2<float> uv2(0.0f, 0.0f); // bottom-left
		//constexpr const Obsidian::Maths::Vec2<float> uv3(1.0f, 0.0f); // bottom-right

		if ((m_Batch.CPUBuffer.size() / 4u) >= MaxQuads) [[unlikely]]
		{
			Game::Instance().OnMessage(MessageType::Warn, std::format("Reached max amount of quads ({0}), to support more either manually change BatchRenderer2D::MaxQuads or contact the developer.", MaxQuads));
			return;
		}

		const float zAxis = position.z * -1.0f;

		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y, zAxis), uv.TopRight, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y, zAxis), uv.TopLeft, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y + size.y, zAxis), uv.BottomLeft, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y + size.y, zAxis), uv.BottomRight, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));

		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y, zAxis), uv1, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y, zAxis), uv0, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y + size.y, zAxis), uv2, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y + size.y, zAxis), uv3, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));

		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y, zAxis), uv0, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y, zAxis), uv1, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y + size.y, zAxis), uv2, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y + size.y, zAxis), uv3, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y, zAxis), uv1, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y, zAxis), uv0, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y + size.y, zAxis), uv3, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y + size.y, zAxis), uv2, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
	
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y, zAxis), uv.BottomRight, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y, zAxis), uv.BottomLeft, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y + size.y, zAxis), uv.TopLeft, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y + size.y, zAxis), uv.TopRight, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
	
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y, zAxis), uv.TopRight, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y, zAxis), uv.TopLeft, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y + size.y, zAxis), uv.BottomLeft, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		//m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y + size.y, zAxis), uv.BottomRight, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
	
		m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y, zAxis), uv.BottomLeft, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y, zAxis), uv.BottomRight, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x + size.x, position.y + size.y, zAxis), uv.TopRight, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
		m_Batch.CPUBuffer.emplace_back(Obsidian::Maths::Vec3<float>(position.x, position.y + size.y, zAxis), uv.TopLeft, Obsidian::Maths::Vec4<float>{ 1.0f, 1.0f, 1.0f, 1.0f }, static_cast<uint32_t>(textureID));
	}

}