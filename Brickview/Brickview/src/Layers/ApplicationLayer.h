#pragma once

#include "Core/Layer/Layer.h"
#include "Core/Event/ApplicationEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Event/KeyEvent.h"

#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Camera.h"
#include "Renderer/CameraController.h"

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
		bool onMouseMoved(const MouseMovedEvent& e);
		bool onKeyPressed(const KeyPressedEvent& e);

	private:
		std::shared_ptr<VertexArray> m_vertexArray;
		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
		std::shared_ptr<Shader> m_colorShader;

		std::shared_ptr<VertexArray> m_pieceVertexArray;
		std::shared_ptr<VertexBuffer> m_pieceVertexBuffer;
		std::shared_ptr<IndexBuffer> m_pieceIndexBuffer;
		std::shared_ptr<Shader> m_pieceShader;

		glm::vec3 m_quadPosition = glm::vec3(0.0f);
		glm::vec3 m_quadScale = glm::vec3(1.0f);

		CameraController m_cameraControl;

		glm::vec3 m_clearColor;
	};

}