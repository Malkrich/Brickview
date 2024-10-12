#pragma once

#include <Brickview.h>

#include "Panels/LegoPartsExplorerPanel.h"

namespace Brickview
{

	class ApplicationLayer : public Layer
	{
	public:
		ApplicationLayer() {}
		virtual ~ApplicationLayer() {}

		virtual void onAttach() override;
		virtual void onDetach() override;

		virtual void onEvent(Brickview::Event& e) override;
		virtual void onUpdate(DeltaTime dt) override;
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

		// Scene
		Ref<Scene> m_scene;

		CameraController m_cameraControl;
		bool m_laptopMode = false;

		DeltaTime m_dt = 0.0f;

		// Panels
		Scope<LegoPartsExplorerPanel> m_legoPartsExplorerPanel;
	};

}