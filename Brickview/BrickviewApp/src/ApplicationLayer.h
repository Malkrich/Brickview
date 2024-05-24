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

		Ref<Mesh> m_planeMesh = nullptr;
		Ref<Mesh> m_cubeMesh = nullptr;
		Ref<Mesh> m_ldrawBrick = nullptr;
		glm::mat4 m_ldrawBrickTransform;

		std::filesystem::path m_ldrawDir;
		const uint32_t m_maxDisplayableFiles = 10;
		uint32_t m_fileIndexOffset = 0;
		uint32_t m_selectedMesh = 1;

		Material m_ldrawBrickMaterial;
		Light m_light;
		RenderType m_renderType = RenderType::Rendered;

		CameraController m_cameraControl;
		bool m_laptopMode = false;

		float m_dt = 0.0f;
	};

}