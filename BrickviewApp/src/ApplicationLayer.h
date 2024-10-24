#pragma once

#include <Brickview.h>

#include "Panels/LegoPartsExplorerPanel.h"
#include "Panels/LegoPartsCollectionPanel.h"

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
		// Scene
		Ref<Scene> m_scene = nullptr;

		// Render
		Ref<SceneRenderer> m_renderer = nullptr;
		uint32_t m_viewportWidth, m_viewportHeight;

		Scope<CameraController> m_cameraControl = nullptr;
		bool m_laptopMode = false;

		// Guizmo tests
		Entity m_selectedEntity = {};

		DeltaTime m_dt = 0.0f;

		// Panels
		Scope<LegoPartsExplorerPanel> m_legoPartsExplorerPanel;
		Scope<LegoPartsCollectionPanel> m_legoPartsCollectionPanel;
	};

}