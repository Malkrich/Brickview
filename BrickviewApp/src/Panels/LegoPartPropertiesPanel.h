#pragma once

#include <Brickview.h>

namespace Brickview
{

	class LegoPartPropertiesPanel
	{
	public:
		LegoPartPropertiesPanel(Entity entity = {});

		Entity getEntityContext() const { return m_entityContext; }
		void setEntityContext(Entity entity) { m_entityContext = entity; }

		void onGuiRender();

		void drawEntityComponents(Entity entity);

	private:
		Entity m_entityContext = {};
	};

}