#pragma once

#include "TextureSpecifications.h"

#include <filesystem>

namespace Brickview
{

	struct Texture2DSpecifications
	{
		TextureFormat Format = TextureFormat::RGB;
		TextureWrapMode WrappingModeU = TextureWrapMode::Repeat;
		TextureWrapMode WrappingModeV = TextureWrapMode::Repeat;
		TextureFilter FilterMin = TextureFilter::Linear;
		TextureFilter FilterMag = TextureFilter::Linear;

		Texture2DSpecifications() = default;
	};

	class Texture2D
	{
	public:
		static Ref<Texture2D> create(const Texture2DSpecifications specs, const std::filesystem::path& filePath);

		virtual ~Texture2D() = default;

		virtual void bind(uint32_t slot = 0) const = 0;

		virtual const Texture2DSpecifications& getSpecifications() const = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		virtual uint32_t getTextureID() const = 0;
	};

}