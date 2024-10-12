#pragma once

#include <filesystem>

namespace Brickview
{

	struct TextureSpecifications
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint8_t Channels = 0;
	};

	class Texture2D
	{
	public:
		Texture2D(const std::filesystem::path& filePath);

		uint32_t getWidth() const { return m_specs.Width; }
		uint32_t getHeight() const { return m_specs.Height; }
		uint32_t getDescriptor() const { return m_textureID; }

	private:
		TextureSpecifications m_specs;
		uint32_t m_textureID = 0;
	};

}