#pragma once

#include <Brickview.h>

namespace Brickview
{

	class LegoPartsCollectionPanel
	{
	public:
		LegoPartsCollectionPanel(Ref<Scene> context);

		void onGuiRender();

	private:
		Ref<Scene> m_sceneContext;
	};

}