#pragma once

#include "Mario/Core/Core.hpp"

#include "Mario/Renderer/Texture.hpp"

#include <Obsidian/Renderer/Image.hpp>
#include <Obsidian/Renderer/Buffer.hpp>
#include <Obsidian/Renderer/Shader.hpp>
#include <Obsidian/Renderer/Pipeline.hpp>
#include <Obsidian/Renderer/Bindings.hpp>
#include <Obsidian/Renderer/Swapchain.hpp>
#include <Obsidian/Renderer/Renderpass.hpp>

#include <cstdint>
#include <array>

namespace Mario
{

	////////////////////////////////////////////////////////////////////////////////////
	// RendererVertex
	////////////////////////////////////////////////////////////////////////////////////
	struct RendererVertex
	{
	public:
		Vec3<float> Position = { 0.0f, 0.0f, 0.0f };
		Vec2<float> UV = { 0.0f, 0.0f };
		Vec4<float> Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
		uint32_t TextureID = 0;
	};

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
	// UV Maps
	////////////////////////////////////////////////////////////////////////////////////
	struct UVMaps
	{
	public:
		struct Mario
		{
		public:
			inline static constexpr UV Standing = UV({ 0.0f, 0.0f }, { 16.0f / 480.0f , 0.0f }, { 0.0f, 16.0f / 620.0f }, { 16.0f / 480.0f, 16.0f / 620.0f });
		};
	};

	////////////////////////////////////////////////////////////////////////////////////
	// Renderer
	////////////////////////////////////////////////////////////////////////////////////
	class Renderer
	{
	public:
		// Matches TextureID in RendererVertex
		enum class TextureID : uint32_t { White = 0, MarioLuigi, EnemiesBosses, ItemsObjects, Tiles, Count };
	public:
		inline static constexpr uint32_t MaxQuads = 10000u;
		inline static constexpr uint32_t MaxTextures = static_cast<uint32_t>(TextureID::Count);
	public:
		// Constructor & Destructor
		Renderer();
		~Renderer();

		// Methods
		void Begin(const Mat4<float>& view, const Mat4<float>& projection);
		void End(Obsidian::CommandList& list, const Vec4<float>& bgColour);

		void Resize(uint32_t width, uint32_t height);

		void DrawQuad(const Vec3<float>& position, const Vec2<float>& size, const UV& uv, TextureID textureID);

	private:
		// Init methods
		void InitMain(Obsidian::CommandList& list);
		void InitBatch(Obsidian::CommandList& list);
		void InitMario(Obsidian::CommandList& list);

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

		struct
		{
			Texture MarioLuigi;
			Texture EnemiesBosses;
			Texture ItemsObjects;
			Texture Tile;
		} m_Sheets;
	};

}