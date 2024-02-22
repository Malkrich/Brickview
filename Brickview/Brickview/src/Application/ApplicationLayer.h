#pragma once

#include <glm/glm.hpp>

#include "Core/Layer/Layer.h"
#include "Core/Event/Event.h"
#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Shader/Shader.h"

namespace Brickview
{

	class ApplicationLayer : public Layer
	{
	public:
		ApplicationLayer() = default;
		virtual ~ApplicationLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;

		virtual void onEvent(Event& e) override;
		virtual void onUpdate(float dt) override;
		virtual void onGuiRender() override;

	private:
		bool onWindowResize(const WindowResizeEvent& e);
		bool onMouseMove(const MouseMoveEvent& e);

	private:
		std::shared_ptr<VertexArray> m_vertexArray;
		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
		std::shared_ptr<Shader> m_colorShader;

		glm::vec3 m_clearColor;
	};

}