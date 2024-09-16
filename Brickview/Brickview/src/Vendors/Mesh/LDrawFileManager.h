#pragma once

#include <filesystem>
#include <tuple>

namespace Brickview
{

	enum class LDrawFileType
	{
		None = 0,
		Part,
		SubPart,
		Primitive,
	};

	enum class LDrawPrimitiveType
	{
		None = 0,
		NoDetails,
		Details
	};

	class LDrawFileManager
	{
	public:
		static void init();
		static void shutdown();

		static std::filesystem::path getFullPartsDirectory();
		static std::filesystem::path getFullSubPartsDirectory();
		static std::filesystem::path getFullPrimitivesDirectory(LDrawPrimitiveType type);

		static std::tuple<std::filesystem::path, LDrawFileType> getFileFromRawFileName(const std::filesystem::path& fileName);

		// For debug
		static std::string fileTypeToString(LDrawFileType type);

	private:
		static std::tuple<std::filesystem::path, LDrawFileType> findFile(const std::filesystem::path& fileName);

		static LDrawFileType fileTypeFromParentDir(const std::filesystem::path& parentDir);
		static LDrawFileType fileTypeFromUnknownFile(const std::filesystem::path& fileName);
		static LDrawPrimitiveType primitiveTypeFromParentDir(const std::filesystem::path& parentDir);
		static LDrawPrimitiveType primitiveTypeFromUnknownFile(const std::filesystem::path& fileName);

		static std::filesystem::path filePathFromTypes(LDrawFileType fileType, LDrawPrimitiveType primitiveType);
		static std::filesystem::path primitiveSubDirFromType(LDrawPrimitiveType type);
	};

}