#pragma once

#include "Renderer/UniformBuffer.h"

namespace Brickview
{

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(const UniformBufferSpecifications& specs);

		virtual ~OpenGLUniformBuffer();

		virtual void setElement(uint32_t elementIndex, const void* data) override;

		virtual const UniformBufferSpecifications& getSpecifications() const override { return m_specs; }

	private:
		uint32_t m_bufferID = 0;
		
		UniformBufferSpecifications m_specs;
	};
	
}