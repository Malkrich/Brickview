#include "Pch.h"
#include "SolidRenderer.h"
#include "Renderer/Renderer/BatchRendererManager.h"
#include "Renderer/Shader/ShaderLibrary.h"
#include "Renderer/Buffer/Layout.h"

namespace Brickview
{
	namespace SolidRendererTypes
	{
		struct MeshVertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
		};

		struct RendererData
		{
			const uint32_t MaxVertices = 1024;
			const uint32_t MaxIndices = MaxVertices;

			// Mesh
			UniformMap MeshUniforms = {};
			std::vector<MeshVertex> MeshVertices;
			std::vector<TriangleFace> MeshIndices;

			// Light
			Light Light;

			// Camera data
			glm::mat4 ViewProjectionMatrix;
			glm::vec3 CameraPosition;

			// Render submissions
			Scope<BatchRendererManager> RendererManager;
		};
	}

	static SolidRendererTypes::RendererData* s_solidRendererData;

	void SolidRenderer::init(const Ref<Shader>& meshShader)
	{
		s_solidRendererData = new SolidRendererTypes::RendererData();

		s_solidRendererData->RendererManager = createScope<BatchRendererManager>();

		// Mesh
		Layout meshLayout = {
			{ "a_position", BufferElementType::Float3 },
			{ "a_normal", BufferElementType::Float3 }
		};

		s_solidRendererData->RendererManager->addSubmission("Meshes",
			s_solidRendererData->MaxVertices, s_solidRendererData->MaxIndices,
			meshLayout,
			meshShader);
	}

	void SolidRenderer::shutdown()
	{
		delete s_solidRendererData;
		s_solidRendererData = nullptr;
	}

	void SolidRenderer::begin(const Camera& camera, const Light& light)
	{
		s_solidRendererData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
		s_solidRendererData->CameraPosition = camera.getPosition();
		s_solidRendererData->Light = light;
	}

	void SolidRenderer::submitMesh(const Ref<Mesh>& mesh, const glm::mat4& transform)
	{
		const auto& vertices = mesh->getVertices();
		const auto& indices = mesh->getIndices();

		if (vertices.size() + s_solidRendererData->MeshVertices.size() > s_solidRendererData->MaxVertices
			|| indices.size() + s_solidRendererData->MeshIndices.size() > s_solidRendererData->MaxIndices)
		{
			flush();
		}

		unsigned int offset = s_solidRendererData->MeshVertices.size();
		s_solidRendererData->MeshVertices.reserve(s_solidRendererData->MeshVertices.size() + vertices.size());
		s_solidRendererData->MeshIndices.reserve(s_solidRendererData->MeshIndices.size() + indices.size());
		for (const auto& v : vertices)
		{
			glm::vec3 p = transform * glm::vec4(v.Position, 1.0);
			s_solidRendererData->MeshVertices.push_back({ p, v.Normal });
		}
		for (const auto& f : indices)
		{
			TriangleFace face;
			face[0] = f[0] + offset;
			face[1] = f[1] + offset;
			face[2] = f[2] + offset;
			s_solidRendererData->MeshIndices.push_back(face);
		}

		s_solidRendererData->MeshUniforms["u_viewProjection"] = s_solidRendererData->ViewProjectionMatrix;
		s_solidRendererData->MeshUniforms["u_cameraPosition"] = s_solidRendererData->CameraPosition;
	}

	void SolidRenderer::flush()
	{
		// Meshes
		if (s_solidRendererData->MeshVertices.size() != 0)
		{
			s_solidRendererData->RendererManager->setData("Meshes",
				s_solidRendererData->MeshVertices.size() * sizeof(SolidRendererTypes::MeshVertex),
				(void*)s_solidRendererData->MeshVertices.data(),
				s_solidRendererData->MeshIndices.size() * sizeof(TriangleFace),
				(void*)s_solidRendererData->MeshIndices.data());
			s_solidRendererData->RendererManager->setUniforms("Meshes", s_solidRendererData->MeshUniforms);

			s_solidRendererData->MeshVertices.clear();
			s_solidRendererData->MeshIndices.clear();
		}

		s_solidRendererData->RendererManager->flush();
	}

	void SolidRenderer::end()
	{
		flush();
	}
}