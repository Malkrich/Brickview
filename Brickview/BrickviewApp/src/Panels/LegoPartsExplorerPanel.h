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

		void setOnLoadCallbackFunction(OnFileLoadCallbackFn fn) { m_onFileLoadCallback = fn; }

		void onGuiRender();

	private:
		uint32_t m_fileOffset = 1000;
		uint32_t m_maxDisplayedFile = 10;
		std::filesystem::path m_legoPartsDirectory;

		OnFileLoadCallbackFn m_onFileLoadCallback;

		Ref<Texture2D> m_defaultLegoPartIcon;
		uint32_t m_iconWidth = 120;
		uint32_t m_iconPadding = 10;
	};

}