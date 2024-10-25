#pragma once

#include <Brickview.h>

#include "Panels/LegoPartsExplorerPanel.h"
#include "Panels/ScenePartsListPanel.h"

#include <imgui.h>
#include <ImGuizmo/ImGuizmo.h>

namespace Brickview
{
	
	class ApplicationLayer : public Layer
	{
	public:
		enum class EditorManipulationType
		{
			None = 0,
			Translate, Rotate
		};

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
		bool onMousePressed(const MousePressedEvent& e);
		bool onKeyPressed(const KeyPressedEvent& e);

		// Gui
		void beginDockspace();
		void endDockspace();

		void onFocusEntity(Entity entity);

	private:
		// Scene
		Ref<Scene> m_scene = nullptr;

		// Render
		Ref<SceneRenderer> m_renderer = nullptr;
		uint32_t m_viewportWidth, m_viewportHeight;

		Scope<CameraController> m_cameraControl = nullptr;
		bool m_laptopMode = false;
		EditorManipulationType m_currentManipulationType = EditorManipulationType::Translate;
		bool m_gizmoVisible = false;
		
		// Mouse
		ImVec2 m_mousePosition;
		ImVec2 m_viewportMinBound;
		ImVec2 m_viewportMaxBound;

		DeltaTime m_dt = 0.0f;

		// Panels
		Scope<LegoPartsExplorerPanel> m_legoPartsExplorerPanel;
		Scope<ScenePartsListPanel> m_scenePartsListPanel;
	};

}