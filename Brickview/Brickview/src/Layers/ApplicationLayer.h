#pragma once

#include "Core/Layer/Layer.h"
#include "Core/Event/ApplicationEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Event/KeyEvent.h"

#include "Renderer/Buffer/Buffer.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Camera.h"
#include "Renderer/CameraController.h"
#include "Renderer/Light.h"

#include "Models/Mesh.h"
#include "Material/Material.h"

namespace Brickview
{

	class ApplicationLayer : public Layer
	{
	public:
		ApplicationLayer() = default;
		virtual ~ApplicationLayer() = default;

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
		glm::vec3 m_legoPiecePosition = glm::vec3(0.0f);
		glm::vec3 m_legoPieceScale = glm::vec3(1.0f);
		std::shared_ptr<Mesh> m_legoPieceMesh;
		Material m_legoPieceMaterial;

		Light m_light;
		CameraController m_cameraControl;
	};

}