#include "Pch.h"
#include "RendererBase.h"

namespace Brickview
{

	void RenderSettings::add(const std::string& name, const RenderParameter& parameter)
	{
		BV_ASSERT(!m_parameters.contains(name), "Parameter already exists!");
		m_parameters[name] = parameter;
	}

}