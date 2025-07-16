#pragma once

#include "Texture.h"
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

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> create(const Texture2DSpecifications specs, const std::filesystem::path& filePath);
		static Ref<Texture2D> create(const Texture2DSpecifications specs, uint32_t width, uint32_t height);
		static Ref<Texture2D> create(const Texture2DSpecifications specs, uint32_t dimensions);

		static Ref<Texture2D> copy(const Texture2DSpecifications& specs, uint32_t sourceTexture, uint32_t width, uint32_t height);
		static Ref<Texture2D> copy(const Texture2DSpecifications& specs, uint32_t sourceTexture, uint32_t dimensions);

		virtual void bind(uint32_t slot = 0) const = 0;

		virtual const Texture2DSpecifications& getSpecifications() const = 0;
	};

}