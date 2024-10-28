#include "Pch.h"
#include "UniformBuffer.h"

#include "Renderer/RendererAPI.h"
#include "Vendors/OpenGL/OpenGLUniformBuffer.h"

#include <glm/glm.hpp>

namespace Brickview
{

	namespace Utils
	{

		static uint32_t getElementSize(UniformBufferElementType type)
		{
			switch (type)
			{
				case UniformBufferElementType::None:   BV_ASSERT(false, "Unknown elemnent type!");
				case UniformBufferElementType::Int:    return sizeof(int);
				case UniformBufferElementType::Float:  return sizeof(float);
				case UniformBufferElementType::Float3: return sizeof(glm::vec3);
				case UniformBufferElementType::Mat4:   return sizeof(glm::mat4);
			}

			BV_ASSERT(false, "UniformBufferElementType size calculation is not implemented yet!");
			return 0;
		}

		// more info on table from https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
		static uint32_t getElementAlignment(UniformBufferElementType type)
		{
			switch (type)
			{
				case UniformBufferElementType::None:   BV_ASSERT(false, "Unknown elemnent type!");
				case UniformBufferElementType::Int:    return 4;
				case UniformBufferElementType::Float:  return 4;
				case UniformBufferElementType::Float2: return 2 * 4;
				case UniformBufferElementType::Float3: return 4 * 4;
				case UniformBufferElementType::Float4: return 4 * 4;
				case UniformBufferElementType::Mat4:   return 4 * 4 * 4;
			}

			BV_ASSERT(false, "UniformBufferElementType size calculation is not implemented yet!");
			return 0;
		}

	}

	UniformBufferLayout::UniformBufferLayout(const std::initializer_list<UniformBufferElement>& elements)
		: m_elements(elements)
	{
		calculateSizeAndElementOffsets();
	}

	void UniformBufferLayout::calculateSizeAndElementOffsets()
	{
		uint32_t offset = 0;
		uint32_t previousAlignment = 0;
		uint32_t previousOffset = 0;
		for (UniformBufferElement& element : m_elements)
		{
			uint32_t elementAlignment = Utils::getElementAlignment(element.Type);

			uint32_t alignmentTest = (previousOffset + elementAlignment) % elementAlignment;
			bool compatible = alignmentTest == 0;

			if (!compatible)
			{
				uint32_t padding = elementAlignment - previousAlignment;
				offset += padding;
				m_bufferSize += padding;
			}

			element.Size = Utils::getElementSize(element.Type);
			element.Offset = offset;

			offset += elementAlignment;
			m_bufferSize += elementAlignment;

			previousAlignment = elementAlignment;
			previousOffset = offset;
		}
	}

	Ref<UniformBuffer> UniformBuffer::create(const UniformBufferSpecifications& specs)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLUniformBuffer>(specs);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

}