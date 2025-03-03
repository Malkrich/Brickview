#pragma once

#include "TextureSpecifications.h"

namespace Brickview
{

	enum class CubemapFace
	{
		PositiveX = 0, NegativeX = 1,
		PositiveY = 2, NegativeY = 3,
		PositiveZ = 4, NegativeZ = 5,
	};

	struct CubemapSpecifications
	{
		uint32_t Width = 0, Height = 0;
		uint32_t Levels = 1;
		TextureFormat Format = TextureFormat::RGBFloat16;
		TextureFilter MinFilter = TextureFilter::Linear;
		TextureFilter MagFilter = TextureFilter::Linear;

		CubemapSpecifications() = default;
	};

	class Cubemap
	{
	public:
		static Ref<Cubemap> create(const CubemapSpecifications& specs);
		static Ref<Cubemap> copy(const CubemapSpecifications& specs, uint32_t textureID);

		virtual void bind(uint32_t slot = 0) const = 0;

		virtual ~Cubemap() = default;
	};

}