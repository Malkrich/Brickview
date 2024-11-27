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

		static void updateCurrentElementSizeAndOffset(UniformBufferElement& currentElement, const UniformBufferElement& nextElement, uint32_t elementCount, uint32_t& offset)
		{
			const std::vector<UniformBufferElementType>& currentElementTypes = currentElement.Types;

			currentElement.Offset = offset;

			uint32_t localOffset = 0;
			for (size_t i = 0; i < currentElement.Types.size() - 1; i++)
			{
				uint32_t currentSize = Utils::getElementSize(currentElementTypes[i]);
				//localSize            += currentSize;
				localOffset += currentSize;

				uint32_t nextAlignment = Utils::getElementAlignment(currentElementTypes[i + 1]);
				if (localOffset % nextAlignment != 0)
				{
					uint32_t padding = nextAlignment - currentSize;
					localOffset += padding;
				}
			}

			uint32_t lastAlignment = Utils::getElementAlignment(currentElementTypes[currentElementTypes.size() - 1]);

			offset += elementCount * localOffset;
		}

	}

	UniformBufferLayout::UniformBufferLayout(const std::initializer_list<UniformBufferElement>& elements)
		: m_elements(elements)
	{
		calculateElementSizesAndOffsets();
	}

	void UniformBufferLayout::calculateElementSizesAndOffsets()
	{
		uint32_t offset = 0;
		for (size_t i = 0; i < m_elements.size() - 1; i++)
		{
			UniformBufferElement& element = m_elements[i];
			const UniformBufferElement& nextElement = m_elements[i + 1];
			Utils::updateCurrentElementSizeAndOffset(element, nextElement, element.ElementCount, offset);
		}

		UniformBufferElement& lastElement = m_elements[m_elements.size() - 1];
		//lastElement.Size = Utils::getElementSize(lastElement.Types[0]);
		//lastElement.Offset = offset;

		//uint32_t lastAlignment = Utils::getElementAlignment(lastElement.Types[0]);
		//m_bufferSize = offset + lastAlignment;
	}

	Ref<UniformBuffer> UniformBuffer::create()
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<UniformBuffer>();
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;

	}

	Ref<UniformBuffer> UniformBuffer::create(uint32_t size, const void* data)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<UniformBuffer>(size, data);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

}