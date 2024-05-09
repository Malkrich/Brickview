#include "Pch.h"
#include "Viewport.h"

#include <imgui.h>

namespace Brickview
{

	Viewport::Viewport(uint32_t width, uint32_t height)
	{
		FrameBufferSpecifications fbSpec;
		fbSpec.Width = width;
		fbSpec.Height = height;
		m_frameBuffer = createScope<FrameBuffer>(fbSpec);
	}

	void Viewport::onGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGui::Begin("Viewport");

		// Hovering part
		m_hovered = ImGui::IsWindowHovered();

		// Resize
		glm::vec2 currentViewportDim = {
			(float)m_frameBuffer->getSpecifications().Width, 
			(float)m_frameBuffer->getSpecifications().Height };
		ImVec2 newViewportDim = ImGui::GetContentRegionAvail();

		m_hasSizeChanged = false;
		if (currentViewportDim.x != newViewportDim.x || currentViewportDim.y != newViewportDim.y)
		{
			m_hasSizeChanged = true;
			if (newViewportDim.x != 0 && newViewportDim.y != 0)
				m_frameBuffer->resize(newViewportDim.x, newViewportDim.y);
		}

		// Viewport render
		uint32_t textureID = m_frameBuffer->getColorAttachment();
		ImGui::Image((void*)textureID, newViewportDim, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

		ImGui::End();
		ImGui::PopStyleVar(3);
	}

}