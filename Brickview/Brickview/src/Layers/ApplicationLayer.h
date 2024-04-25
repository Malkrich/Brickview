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
		ApplicationLayer();
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
		std::shared_ptr<Mesh> m_legoPieceMesh;

		glm::vec3 m_legoPiecePosition1;
		Material m_legoPieceMaterial1;
		glm::vec3 m_legoPiecePosition2;
		Material m_legoPieceMaterial2;

		Light m_light;
		CameraController m_cameraControl;

		float m_dt = 0.0f;
	};

}