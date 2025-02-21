#pragma once

#include "Renderer/UniformBuffer.h"

namespace Brickview
{

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(const UniformBufferSpecifications& specs, uint32_t size);
		OpenGLUniformBuffer(const UniformBufferSpecifications& specs, uint32_t size, const void* data);

		virtual ~OpenGLUniformBuffer();

		virtual void setData(const void* data) override;
		virtual void resize(uint32_t newSize) override;

		virtual const UniformBufferSpecifications& getSpecifications() const override { return m_specs; }

	private:
		void invalidate(uint32_t size, const void* data);

	private:
		uint32_t m_bufferID = 0;
		uint32_t m_bufferSize = 0;

		UniformBufferSpecifications m_specs;
	};
	
}