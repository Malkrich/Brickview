#pragma once

namespace Brickview
{

	enum class TextureFormat
	{
		RGB, RGBA,
		RGBFloat16, RGBFLoat32
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