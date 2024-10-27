#pragma once

#include <Brickview.h>

namespace Brickview
{

	class ScenePartsListPanel
	{
	public:
		ScenePartsListPanel(Ref<Scene> context);

		const Ref<Scene>& getContext() const { return m_sceneContext; }

		void onGuiRender();

	private:
		Ref<Scene> m_sceneContext;
	};

}