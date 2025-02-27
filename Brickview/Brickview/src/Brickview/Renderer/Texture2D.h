#pragma once

#include <filesystem>

namespace Brickview
{

	enum class Texture2DFormat
	{
		RGB, RGBA, FloatRGB
	};

	enum class Texture2DFilter
	{
		Linear, Nearest
	};

	enum class Texture2DWrappingMode
	{
		Repeat, Clamp
	};

	struct Texture2DSpecifications
	{
		Texture2DFormat Format = Texture2DFormat::RGB;
		Texture2DWrappingMode WrappingModeU = Texture2DWrappingMode::Repeat;
		Texture2DWrappingMode WrappingModeV = Texture2DWrappingMode::Repeat;
		Texture2DFilter FilterMin = Texture2DFilter::Linear;
		Texture2DFilter FilterMag = Texture2DFilter::Linear;

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