#pragma once

#include "Mario/Core/Core.hpp"

#include <Obsidian/Renderer/Image.hpp>
#include <Obsidian/Renderer/Buffer.hpp>
#include <Obsidian/Renderer/Shader.hpp>
#include <Obsidian/Renderer/Pipeline.hpp>
#include <Obsidian/Renderer/Bindings.hpp>
#include <Obsidian/Renderer/Swapchain.hpp>
#include <Obsidian/Renderer/Renderpass.hpp>

#include <Obsidian/Maths/Structs.hpp>

#include <cstdint>
#include <array>

namespace Mario
{

	class Resources;

	////////////////////////////////////////////////////////////////////////////////////
	// RendererVertex
	////////////////////////////////////////////////////////////////////////////////////
	struct RendererVertex
	{
	public:
		Obsidian::Maths::Vec3<float> Position = { 0.0f, 0.0f, 0.0f };
		Obsidian::Maths::Vec2<float> UV = { 0.0f, 0.0f };
		Obsidian::Maths::Vec4<float> Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
		uint32_t TextureID = 0;
	};

	////////////////////////////////////////////////////////////////////////////////////
	// RendererCamera
	////////////////////////////////////////////////////////////////////////////////////
	struct RendererCamera
	{
	public:
		Obsidian::Maths::Mat4<float> View = Obsidian::Maths::Mat4<float>(1.0f);
		Obsidian::Maths::Mat4<float> Projection = Obsidian::Maths::Mat4<float>(1.0f);
	};

	////////////////////////////////////////////////////////////////////////////////////
	// TextureID
	////////////////////////////////////////////////////////////////////////////////////
	enum class TextureID : uint8_t
	{
		White = 0, MarioLuigi, EnemiesBosses, ItemsObjects, Tiles, Count
	};

	////////////////////////////////////////////////////////////////////////////////////
	// Renderer
	////////////////////////////////////////////////////////////////////////////////////
	class Renderer
	{
	public:
		inline static constexpr uint32_t MaxQuads = 10000u;
		inline static constexpr uint32_t MaxTextures = static_cast<uint32_t>(TextureID::Count);
	public:
		// Constructor & Destructor
		Renderer(const Resources& resources);
		~Renderer();

		// Methods
		void Begin();
		void End();
		void Flush(Obsidian::CommandList& list);

		void Resize(uint32_t width, uint32_t height);

	private:
		// Init methods
		void InitMain(Obsidian::CommandList& list);
		void InitBatch(Obsidian::CommandList& list);
		void InitMario(Obsidian::CommandList& list);

		// Private methods

	private:
		DeferredConstruct<Obsidian::Image> m_WhiteTexture = {};
		DeferredConstruct<Obsidian::Sampler> m_TextureSampler = {};
		DeferredConstruct<Obsidian::Buffer> m_CameraBuffer = {};
		
		struct
		{
			DeferredConstruct<Obsidian::InputLayout> InputLayout = {};
			DeferredConstruct<Obsidian::BindingLayout> BindingLayoutSet0 = {};
			DeferredConstruct<Obsidian::GraphicsPipeline> Pipeline = {};

			DeferredConstruct<Obsidian::BindingSetPool> Set0Pool = {};
			DeferredConstruct<Obsidian::BindingSet> Set0 = {};

			DeferredConstruct<Obsidian::Renderpass> Renderpass = {};

			DeferredConstruct<Obsidian::Buffer> VertexBuffer = {};
			DeferredConstruct<Obsidian::Buffer> IndexBuffer = {};

			std::vector<RendererVertex> CPUBuffer = { };
		} m_Batch;

		const Resources& m_Resources;
	};

}