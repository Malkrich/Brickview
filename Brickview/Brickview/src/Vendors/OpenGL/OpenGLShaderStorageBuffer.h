#pragma once
#include "Renderer/ShaderStorageBuffer.h"

namespace Brickview
{

	class OpenGLShaderStorageBuffer : public ShaderStorageBuffer
	{
	public:
		OpenGLShaderStorageBuffer(const ShaderStorageBufferSpecifications& specs, uint32_t size, const void* data);
		OpenGLShaderStorageBuffer(const ShaderStorageBufferSpecifications& specs, uint32_t size);
		virtual ~OpenGLShaderStorageBuffer();
		
		virtual uint32_t getSize() const override { return m_bufferSize; }

		virtual void resize(uint32_t size) override;
		virtual void setData(const void* data) override;

	private:
		void invalidate(uint32_t size, const void* data);

	private:
		uint32_t m_bufferID = 0;
		uint32_t m_bufferSize = 0;

		ShaderStorageBufferSpecifications m_specs;
	};

}