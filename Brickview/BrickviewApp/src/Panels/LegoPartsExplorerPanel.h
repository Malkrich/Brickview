#pragma once

#include <Brickview.h>

#include <filesystem>
#include <functional>

namespace Brickview
{

	class LegoPartsExplorerPanel
	{
	public:
		typedef std::function<void(const std::filesystem::path&)> OnFileLoadCallbackFn;

	public:
		LegoPartsExplorerPanel(const std::filesystem::path& partsDirectory);

		void setOnLoadCallback(OnFileLoadCallbackFn fn) { m_onFileLoadCallback = fn; }

		void onImGuiRender();

	private:
		std::filesystem::path m_legoPartsDirectory;
		OnFileLoadCallbackFn m_onFileLoadCallback;

		Ref<Texture2D> m_defaultLegoPartIcon;
	};

}