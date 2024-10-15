#include "Pch.h"
#include "UniformBuffer.h"

#include "Renderer/RendererAPI.h"
#include "Vendors/OpenGL/OpenGLUniformBuffer.h"

#include <glm/glm.hpp>

namespace Brickview
{

	namespace Utils
	{

		static uint32_t getElementSizeFromType(UniformBufferElementType type)
		{
			switch (type)
			{
				case UniformBufferElementType::None:   BV_ASSERT(false, "Unknown elemnent type!");
				case UniformBufferElementType::Float3: return sizeof(glm::vec3);
				case UniformBufferElementType::Mat4:   return sizeof(glm::mat4);
			}

			BV_ASSERT(false, "UniformBufferElementType size calculation is not implemented yet!");
			return 0;
		}

	}

	UniformBufferLayout::UniformBufferLayout(std::initializer_list<UniformBufferElement> elements)
		: m_elements(elements)
	{
		calculateSizeAndElementOffsets();
	}

	void UniformBufferLayout::calculateSizeAndElementOffsets()
	{
		uint32_t offset = 0;
		for (UniformBufferElement& element : m_elements)
		{
			uint32_t elementSize = Utils::getElementSizeFromType(element.Type);
			m_bufferSize         += elementSize;

			element.Size         = elementSize;
			element.Offset       = offset;

			offset               += elementSize;
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