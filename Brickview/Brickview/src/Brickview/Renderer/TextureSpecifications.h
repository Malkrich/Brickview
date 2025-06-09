#pragma once

namespace Brickview
{

	enum class TextureFormat
	{
		RGBInt32, RGBAInt32,
		RGFloat16,
		RGBFloat16, RGBFLoat32,

		// Defaults
		RGB = RGBInt32,
		RGBA = RGBAInt32
	};

	enum class TextureFilter
	{
		Linear, Nearest,
		LinearMipmapLinear
	};

	enum class TextureWrapMode
	{
		Repeat, ClampToEdge
	};

}