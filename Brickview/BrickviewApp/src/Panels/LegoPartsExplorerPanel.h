#pragma once

#include <Brickview.h>

#include <filesystem>
#include <functional>

namespace Brickview
{

	class LegoPartsExplorerPanel
	{
	public:
		typedef std::function<void(Ref<Mesh>)> OnFileLoadCallbackFn;

	public:
		LegoPartsExplorerPanel(const std::filesystem::path& partsDirectory);

		void setOnLoadCallback(OnFileLoadCallbackFn fn) { m_onFileLoadCallback = fn; }

		void onImGuiRender();

	private:
		uint32_t m_fileOffset = 1000;
		uint32_t m_maxDisplayedFile = 10;
		std::filesystem::path m_legoPartsDirectory;

		OnFileLoadCallbackFn m_onFileLoadCallback;

		Ref<Texture2D> m_defaultLegoPartIcon;
	};

}