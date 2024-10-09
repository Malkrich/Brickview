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

		Ref<Mesh> m_legoBrick = nullptr;
		Material m_legoBrickMaterial;
		std::vector<glm::mat4> m_legoBrickTransforms;

		// Lego file explorer
#ifdef DEV_BRANCH
		std::filesystem::path m_ldrawBaseDir = "./data/LDraw/parts/";
		uint32_t m_selectedMesh = 979;
		uint32_t m_maxDisplayableFiles = 10;
		uint32_t m_fileIndexOffset;
#else
		std::filesystem::path m_ldrawBaseDir = "./data/Meshes/LDrawExample/parts/";
#endif

		// Lights
		Light m_light;

		CameraController m_cameraControl;
		bool m_laptopMode = false;

		float m_dt = 0.0f;
	};

}