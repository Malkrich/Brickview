#pragma once

#include "Renderer/Camera.h"
#include "Renderer/Lights.h"
#include "Geometry/Mesh.h"
#include "Renderer/RendererMaterial.h"
#include "Lego/LegoPart.h"
#include "Lego/LegoMaterial.h"
#include "Lego/LegoPartMeshRegistry.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <filesystem>

namespace Brickview
{

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 getTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct LegoPartComponent
	{
		LegoPartID ID;
		// TODO: Lego material that generates a premade PBR material
		//LegoMaterial Material;

		LegoPartComponent(LegoPartID id, LegoPartMeshRegistry& legoMeshRegistry, Ref<Mesh> mesh)
			: ID(id)
		{
			legoMeshRegistry.addPart(id, mesh);
		}
		LegoPartComponent(const LegoPartComponent&) = default;
	};

	struct MeshComponent
	{
		Ref<GpuMesh> MeshData = nullptr;

		MeshComponent(Ref<Mesh> mesh)
		{
			MeshData = createRef<GpuMesh>(mesh);
		}
		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
	};

	struct LightComponent
	{
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		//float Strength
		//float Radius

		LightComponent(const glm::vec3& color)
			: Color(color)
		{}
		LightComponent() = default;
		LightComponent(LightComponent&) = default;
	};

	struct MaterialComponent
	{
		RendererMaterial Material;

		MaterialComponent() = default;
		MaterialComponent(const RendererMaterial& material)
			: Material(material)
		{}
		MaterialComponent(const MaterialComponent&) = default;
	};

}