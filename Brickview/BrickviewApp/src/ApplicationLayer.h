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

		Ref<Mesh> m_ldrawBrick;

		std::filesystem::path m_ldrawDir = "data/LDraw/p/";
		const uint32_t m_maxDisplayableFiles = 10;
		uint32_t m_fileIndexOffset = 0;
		uint32_t m_selectedMesh = 1;

		Material m_ldrawBrickMaterial;

		Light m_light;
		CameraController m_cameraControl;
		RenderType m_renderType = RenderType::Rendered;

		float m_dt = 0.0f;
	};

}