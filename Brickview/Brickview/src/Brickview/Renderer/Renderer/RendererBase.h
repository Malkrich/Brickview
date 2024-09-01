#pragma once

#include "Core/BasicTypes.h"
#include "Renderer/Shader/ShaderLibrary.h"
#include "Renderer/Camera.h"
#include "Renderer/Light.h"
#include "Renderer/Light.h"
#include "Renderer/Material.h"
#include "Mesh/Mesh.h"

#include <glm/glm.hpp>

namespace Brickview
{

	struct RenderParameter
	{
		RenderParameter()
			: Type(BasicTypes::None)
			, Data(nullptr)
		{}
		RenderParameter(const bool* data)
			: Type(BasicTypes::Bool)
			, Data((void*)data)
		{}

		BasicTypes Type;
		void* Data;
	};

	class RenderSettings
	{
	public:
		template<typename T>
		const T& get(const std::string& name) const
		{
			BV_ASSERT(m_parameters.contains(name), "Parameter doesn't exist!");

			const RenderParameter& param = m_parameters.at(name);
			return *((T*)param.Data);
		}

		void add(const std::string& name, const RenderParameter& parameter);

		template<typename T>
		void add(const std::string& name, const T* data)
		{
			RenderParameter param(data);
			add(name, param);
		}

		template<typename T>
		void set(const std::string& name, const T& value)
		{
			BV_ASSERT(m_parameters.contains(name), "Parameter doesn't exist!");

			T* ptr = (T*)m_parameters[name].Data;
			*ptr = value;
		}

		std::unordered_map<std::string, RenderParameter>::const_iterator begin() const { return m_parameters.begin(); }
		std::unordered_map<std::string, RenderParameter>::const_iterator end() const { return m_parameters.end(); }
		std::unordered_map<std::string, RenderParameter>::iterator begin() { return m_parameters.begin(); }
		std::unordered_map<std::string, RenderParameter>::iterator end() { return m_parameters.end(); }

	private:
		std::unordered_map<std::string, RenderParameter> m_parameters;
	};

	class RendererBase
	{
	public:
		virtual ~RendererBase() = default;

		virtual const Ref<RenderSettings>& getRenderSettings() const = 0;

		virtual void begin(const Camera& camera, const Light& light) = 0;
		virtual void end() = 0;

		virtual void drawMesh(const Ref<Mesh>& mesh, const Material& material, const glm::mat4& transform) = 0;
		virtual void drawLights(const Light& light) = 0;
	};

}