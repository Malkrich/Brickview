#include "Pch.h"
#include "ImGuiLayer.h"
#include "Core/Application.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <ImGuizmo/ImGuizmo.h>

namespace Brickview
{
	ImGuiLayer::ImGuiLayer()
	{
	}

	void ImGuiLayer::onAttach()
	{
		// IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// GLFW
		GLFWwindow* window = (GLFWwindow*)Application::get()->getWindow()->getLibWindow();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		// ImGuizmo
		ImGuizmo::AllowAxisFlip(false);
	}

	void ImGuiLayer::onDetach()
	{
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::onEvent(Event& e)
	{
		if (m_blockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			bool isHandle = false;
			isHandle |= e.isInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			isHandle |= e.isInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
			e.setIsHandle(isHandle);
		}
	}

	void ImGuiLayer::begin()
	{
		// IMGUI TEST
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		ImGuiIO& io = ImGui::GetIO();
		const Application* app = Application::get();
		io.DisplaySize = ImVec2((float)app->getWindow()->getWidth(),
								(float)app->getWindow()->getHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
		m_time = time;
	}

	void ImGuiLayer::end()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* window = (GLFWwindow*)Application::get()->getWindow()->getLibWindow();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(window);
		}
	}
}