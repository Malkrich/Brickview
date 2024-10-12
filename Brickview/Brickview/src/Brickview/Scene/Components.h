#pragma once

#include "Renderer/Camera.h"
#include "Renderer/Light.h"
#include "Mesh/Mesh.h"

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

		glm::mat4 getTransform()
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct LegoPartComponent
	{
		// TODO:
		//LegoPartID ID;
		//LegoColor Color;

		// TODO: move mesh object to Mesh collection
		Ref<Mesh> Mesh;

		// TEMP: shouldn't load here
		LegoPartComponent(const std::filesystem::path& filePath)
			: Mesh(Mesh::load(filePath))
		{}
		LegoPartComponent(const LegoPartComponent&) = default;
	};

}