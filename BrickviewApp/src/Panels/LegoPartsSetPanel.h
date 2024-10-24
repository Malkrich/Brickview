#pragma once

#include <Brickview.h>

namespace Brickview
{

	class LegoPartsSetPanel
	{
	public:
		LegoPartsSetPanel(Ref<Scene> context);

		Entity getSelectedEntity() const { return m_selectedEntity; }
		void setSelectedEntity(int32_t entityID);

		void onGuiRender();

	private:
		Entity m_selectedEntity = {};

		Ref<Scene> m_sceneContext;
	};

}