#include "Pch.h"
#include "SolidRenderer.h"
#include "Renderer/Shader/ShaderLibrary.h"
#include "Renderer/Buffer/Layout.h"

namespace Brickview
{

	SolidRenderer::SolidRenderer(const Ref<ShaderLibrary>& shaderLib)
		: m_viewProjectionMatrix(glm::mat4(1.0f))
		, m_cameraPosition(glm::vec3(0.0f))
	{
		m_rendererManager = createScope<BatchRendererManager>();

		// Mesh
		Layout meshLayout = {
			{ "a_position", BufferElementType::Float3 },
			{ "a_normal", BufferElementType::Float3 }
		};

		m_rendererManager->addSubmission("Meshes",
			m_maxVertices, m_maxIndices,
			meshLayout,
			shaderLib->get("Solid"));

		// Settings
		m_settings = createRef<RenderSettings>();
		m_settings->add<bool>("Show normals", m_showNormals);
	}

	SolidRenderer::~SolidRenderer()
	{
	}

	void SolidRenderer::begin(const Camera& camera, const Light& light)
	{
		m_viewProjectionMatrix = camera.getViewProjectionMatrix();
		m_cameraPosition       = camera.getPosition();
		m_light                = light;
	}

	void SolidRenderer::drawMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform)
	{
		const auto& vertices = mesh->getVertices();
		const auto& indices = mesh->getIndices();

		if (vertices.size() + m_meshVertices.size() > m_maxVertices
			|| indices.size() + m_meshIndices.size() > m_maxIndices)
		{
			flush();
		}

		unsigned int offset = m_meshVertices.size();
		m_meshVertices.reserve(m_meshVertices.size() + vertices.size());
		m_meshIndices.reserve(m_meshIndices.size() + indices.size());
		for (const auto& v : vertices)
		{
			glm::vec3 p = transform * glm::vec4(v.Position, 1.0);
			m_meshVertices.push_back({ p, v.Normal });
		}
		for (const auto& f : indices)
		{
			TriangleFace face;
			face[0] = f[0] + offset;
			face[1] = f[1] + offset;
			face[2] = f[2] + offset;
			m_meshIndices.push_back(face);
		}

		m_meshUniforms["u_viewProjection"] = m_viewProjectionMatrix;
		m_meshUniforms["u_cameraPosition"] = m_cameraPosition;
	}

	void SolidRenderer::drawLights(const Light& light)
	{
	}

	void SolidRenderer::flush()
	{
		m_meshUniforms["u_showNormals"] = m_settings->get<bool>("Show normals");

		// Meshes
		m_rendererManager->setData("Meshes",
			m_meshVertices.size() * sizeof(SolidRendererTypes::MeshVertex),
			(void*)m_meshVertices.data(),
			m_meshIndices.size() * sizeof(TriangleFace),
			(void*)m_meshIndices.data());
		m_rendererManager->setUniforms("Meshes", m_meshUniforms);

		if (m_meshVertices.size() != 0)
		{
			m_meshVertices.clear();
			m_meshIndices.clear();
		}

		m_rendererManager->flush();
	}

	void SolidRenderer::end()
	{
		flush();
	}

}