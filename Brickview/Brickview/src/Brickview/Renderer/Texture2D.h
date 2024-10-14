#pragma once

#include <filesystem>

namespace Brickview
{

	struct TextureSpecifications
	{
		uint32_t Width;
		uint32_t Height;
		uint8_t Channels = 4;
	};

	class Texture2D
	{
	public:
		static Ref<Texture2D> create(const TextureSpecifications& specs);
		static Ref<Texture2D> create(const std::filesystem::path& filePath);

		virtual ~Texture2D() = default;

		virtual const TextureSpecifications& getSpecifications() const = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		virtual uint32_t getTextureID() const = 0;
	};

}