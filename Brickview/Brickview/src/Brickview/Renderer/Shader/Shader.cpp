#include "Pch.h"
#include "Shader.h"

#include "Renderer/RendererAPI.h"
#include "Vendors/OpenGL/OpenGLShader.h"

namespace Brickview
{

	Ref<Shader> Shader::create(const std::filesystem::path& filePath)
	{
		switch (RendererAPI::getAPI())
		{
			case RendererAPI::API::None:   BV_ASSERT(false, "Brickview does not support RendererAPI::None!");  return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLShader>(filePath);
		}

		BV_ASSERT(false, "Unknown API!");
		return nullptr;
	}

}