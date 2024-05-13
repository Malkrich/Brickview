#pragma once

#include <Brickview.h>

namespace Brickview
{

	class ApplicationLayer : public Layer
	{
	public:
		ApplicationLayer();
		virtual ~ApplicationLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;

		virtual void onEvent(Brickview::Event& e) override;
		virtual void onUpdate(float dt) override;
		virtual void onGuiRender() override;

	private:
		// Events
		bool onWindowResize(const WindowResizeEvent& e);
		bool onMouseMoved(const MouseMovedEvent& e);
		bool onKeyPressed(const KeyPressedEvent& e);

		// Gui
		void beginDockspace();
		void endDockspace();

	private:
		Scope<Viewport> m_viewport = nullptr;

		Ref<Mesh> m_legoPieceMesh;
		Ref<Mesh> m_planeMesh;
		Ref<Mesh> m_ldrawBrick;

		glm::vec3 m_legoPiecePosition1;
		Material m_legoPieceMaterial1;
		glm::vec3 m_legoPiecePosition2;
		Material m_legoPieceMaterial2;

		Light m_light;
		CameraController m_cameraControl;
		bool m_solidView = false;

		float m_dt = 0.0f;
	};

}