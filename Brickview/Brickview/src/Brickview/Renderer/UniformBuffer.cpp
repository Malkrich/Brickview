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
				case UniformBufferElementType::Float2: return sizeof(glm::vec2);
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
		for (size_t i = 0; i < m_elements.size() - 1; i++)
		{
			UniformBufferElement& element = m_elements[i];

			uint32_t currentSize = Utils::getElementSize(element.Type);
			element.Size = currentSize;
			element.Offset = offset;
			offset += currentSize;

			uint32_t nextAlignment = Utils::getElementAlignment(m_elements[i+1].Type);
			if (offset % nextAlignment != 0)
			{
				uint32_t padding = nextAlignment - currentSize;
				offset += padding;
			}
		}

		UniformBufferElement& lastElement = m_elements[m_elements.size() - 1];
		lastElement.Size = Utils::getElementSize(lastElement.Type);
		lastElement.Offset = offset;

		uint32_t lastAlignment = Utils::getElementAlignment(lastElement.Type);
		m_bufferSize = offset + lastAlignment;
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