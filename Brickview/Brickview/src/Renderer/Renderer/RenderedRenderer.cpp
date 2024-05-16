#include "Pch.h"
#include "RenderedRenderer.h"
#include "Renderer/Buffer/Layout.h"

#include <glm/gtc/type_ptr.hpp>

namespace Brickview
{

	RenderedRenderer::RenderedRenderer(const Ref<ShaderLibrary>& shaderLib)
	{
		m_rendererManager = createScope<BatchRendererManager>();

		// Mesh
		Layout meshLayout = {
			{ "a_position", BufferElementType::Float3 },
			{ "a_normal", BufferElementType::Float3 },
			{ "a_color", BufferElementType::Float3 }
		};
		m_rendererManager->addSubmission("Meshes",
			m_maxVertices, m_maxIndices,
			meshLayout,
			shaderLib->get("LegoPiece"));

		// Light
		Layout lightLayout = {
			{ "a_position", BufferElementType::Float3 }
		};
		m_rendererManager->addSubmission("Lights",
			m_maxVertices, m_maxIndices,
			lightLayout,
			shaderLib->get("Light"));

		m_lightMesh = Mesh::load("data/Models/Cube.obj");
	}

	RenderedRenderer::~RenderedRenderer()
	{
	}

	void RenderedRenderer::begin(const Camera& camera, const Light& light)
	{
		m_viewProjectionMatrix = camera.getViewProjectionMatrix();
		m_cameraPosition       = camera.getPosition();
		m_light                = light;

		m_statistics.DrawCalls = 0;
	}

	void RenderedRenderer::drawMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform)
	{
		const auto& vertices = mesh->getVertices();
		const auto& indices  = mesh->getIndices();

		if (vertices.size() + m_meshVertices.size() > m_maxVertices
			|| indices.size() + m_meshIndices.size() > m_maxIndices)
		{
			flush();
		}

		uint32_t offset = m_meshVertices.size();
		m_meshVertices.reserve(m_meshVertices.size() + vertices.size());
		m_meshIndices.reserve(m_meshIndices.size() + indices.size());
		for (const auto& v : vertices)
		{
			glm::vec3 p = transform * glm::vec4(v.Position, 1.0);
			m_meshVertices.push_back({ p, v.Normal, material.Color });
		}
		for (const auto& f : indices)
		{
			TriangleFace face = { f[0], f[1], f[2] };
			face.addOffset(offset);
			m_meshIndices.push_back(face);
		}
		
		// Update stats
		m_statistics.MeshVertexCount       = m_meshVertices.size();
		m_statistics.MeshIndicesCount      = m_meshIndices.size() * 3;

		m_meshUniforms["u_viewProjection"] = m_viewProjectionMatrix;
		m_meshUniforms["u_cameraPosition"] = m_cameraPosition;
		m_meshUniforms["u_lightPosition"]  = m_light.Position;
		m_meshUniforms["u_lightColor"]     = m_light.Color;
	}

	void RenderedRenderer::drawLights(const Light& light)
	{
		static const float lightScale = 0.1f;
		glm::mat4 lightTransform = glm::translate(glm::mat4(1.0f), light.Position)
			* glm::scale(glm::mat4(1.0f), lightScale * glm::vec3(1.0f));

		const auto& vertices = m_lightMesh->getVertices();
		const auto& indices  = m_lightMesh->getIndices();

		uint32_t offset = m_lightVertices.size();
		m_lightVertices.reserve(m_lightVertices.size() + vertices.size());
		m_lightIndices.reserve(m_lightIndices.size() + indices.size());
		for (const auto& v : vertices)
		{
			glm::vec3 p = lightTransform * glm::vec4(v.Position, 1.0f);
			m_lightVertices.push_back({ p });
		}
		for (const auto& f : indices)
		{
			TriangleFace face;
			face[0] = f[0] + offset;
			face[1] = f[1] + offset;
			face[2] = f[2] + offset;
			m_lightIndices.push_back(face);
		}

		m_lightUniforms["u_viewProjection"] = m_viewProjectionMatrix;
		m_lightUniforms["u_lightColor"]     = m_light.Color;
	}

	void RenderedRenderer::flush()
	{
		// Meshes
		m_rendererManager->setData("Meshes",
			m_meshVertices.size() * sizeof(RenderedRendererTypes::MeshVertex),
			(void*)m_meshVertices.data(),
			m_meshIndices.size() * sizeof(TriangleFace),
			(void*)m_meshIndices.data());
		m_rendererManager->setUniforms("Meshes", m_meshUniforms);

		if (m_meshVertices.size() != 0)
		{
			m_meshVertices.clear();
			m_meshIndices.clear();
			m_statistics.DrawCalls++;
		}

		// Lights
		m_rendererManager->setVisible("Lights", true);
		m_rendererManager->setData("Lights",
			m_lightVertices.size() * sizeof(RenderedRendererTypes::LightVertex),
			(void*)m_lightVertices.data(),
			m_lightIndices.size() * sizeof(TriangleFace),
			(void*)m_lightIndices.data());
		m_rendererManager->setUniforms("Lights", m_lightUniforms);

		if (m_lightVertices.size() != 0)
		{
			m_lightVertices.clear();
			m_lightIndices.clear();
			m_statistics.DrawCalls++;
		}
		else
			m_rendererManager->setVisible("Lights", false);

		m_rendererManager->flush();
	}

	void RenderedRenderer::end()
	{
		flush();
	}

	//const RenderSatistics& RenderedRenderer::getStats()
	//{
	//	return s_renderedRendererData->Statistics;
	//}

}