#pragma once

#include <Brickview.h>

namespace Brickview
{

	// Temp (has to be moved in ECS section)
	struct Transform
	{
		Transform(const glm::vec3& translation)
			: Translation(translation)
		{}

		glm::vec3 Translation;

		glm::mat4 getTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Translation);
		}
	};

	class ApplicationLayer : public Layer
	{
	public:
		ApplicationLayer() {}
		virtual ~ApplicationLayer() {}

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

		// LDraw
		std::filesystem::path m_ldrawBaseDir = "./data/LDraw/parts/";
		// Marge
		Ref<Mesh> m_margeMesh = nullptr;
		Material m_margeMeshMaterial;
		std::vector<Transform> m_margeMeshTransforms;
		// Shelf
		Ref<Mesh> m_shelfMesh = nullptr;
		Material m_shelfMeshMaterial;
		std::vector<Transform> m_shelfMeshTransforms;

		// Lights
		Light m_light;

		// Scene
		Ref<Scene> m_scene;

		CameraController m_cameraControl;
		bool m_laptopMode = false;

		float m_dt = 0.0f;
	};

}