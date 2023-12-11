#include "Pch.h"
#include "GuiRenderer.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Core/Application/Application.h"

namespace Brickview
{
	GuiRenderer::GuiRenderer()
	{
		// IMGUI
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewPortsEnable;
		ImGui::StyleColorsDark();
		// GLFW
		GLFWwindow* window = (GLFWwindow*)Application::get()->getWindow()->getLibWindow();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	GuiRenderer::~GuiRenderer()
	{
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}

	void GuiRenderer::onNewFrame()
	{
		// IMGUI TEST
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void GuiRenderer::onRender()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}