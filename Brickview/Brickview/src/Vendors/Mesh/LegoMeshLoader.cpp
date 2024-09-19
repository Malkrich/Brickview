#include "Pch.h"
#include "LegoMeshLoader.h"

#include "Utils/StringUtils.h"
#include "LDrawReader.h"
#include "LDrawCommandManager.h"

#include <glm/glm.hpp>

namespace Brickview
{

	LoadingQueueFileData::LoadingQueueFileData(const std::filesystem::path& filePath, LDrawFileType type, const glm::mat4& transform)
		: FilePath(filePath)
		, Transform(transform)
	{}

	bool LegoMeshLoader::load(const std::filesystem::path& filePath, Ref<Mesh> mesh)
	{
		if (!std::filesystem::exists(filePath))
		{
			BV_LOG_ERROR("Couldn't load: {}", filePath.generic_string());
			return false;
		}

		LoadingSettings settings;
		std::queue<LoadingQueueFileData> loadingQueue;
		LoadingQueueFileData initialFile(filePath, LDrawFileType::Part, glm::mat4(1.0f));
		loadingQueue.push(initialFile);

		while (!loadingQueue.empty())
		{
			const LoadingQueueFileData& file = loadingQueue.front();
			readFile(file, mesh, settings, loadingQueue);
			loadingQueue.pop();
		}

		// TODO: switch all points y coord (LDraw is -y vertical)
		convertToDm(mesh);

		return true;
	}

	bool LegoMeshLoader::readFile(const LoadingQueueFileData& file, Ref<Mesh> mesh, 
		LoadingSettings& settings, std::queue<LoadingQueueFileData>& loadingQueue)
	{
		std::filesystem::path currentFilePath = file.FilePath;
		const glm::mat4& currentTransform     = file.Transform;

		LDrawReader reader(currentFilePath);

		if (!reader.isValid())
		{
			BV_LOG_ERROR("Couldn't read file in {}", currentFilePath.generic_string());
			return false;
		}

		while (reader.readLine())
		{
			LDrawLineType lineType = reader.getLineType();

			switch (lineType)
			{
				case LDrawLineType::Triangle:
				{
					LDrawTriangleData t = reader.getLineData<LDrawTriangleData>();
					if (!settings.Inverted)
						mesh->addTriangle(t.p0, t.p1, t.p2, currentTransform);
					else
						mesh->addTriangle(t.p2, t.p1, t.p0, currentTransform);
					break;
				}
				case LDrawLineType::Quadrilateral:
				{
					LDrawQuadData q = reader.getLineData<LDrawQuadData>();
					if (!settings.Inverted)
						mesh->addQuad(q.p0, q.p1, q.p2, q.p3, currentTransform);
					else
						mesh->addQuad(q.p3, q.p2, q.p1, q.p0, currentTransform);
					break;
				}
				case LDrawLineType::SubFileRef:
				{
					LDrawSubFileRefData sf = reader.getLineData<LDrawSubFileRefData>();
					LDrawFileData fileData = LDrawFileManager::getFileFromRawFileName(sf.FilePath);

					if (fileData.FileType != LDrawFileType::None)
					{
						BV_LOG_INFO("Adding file {} to the loading queue", fileData.FilePath.string());

						LoadingQueueFileData newFile;
						newFile.FilePath  = fileData.FilePath;
						newFile.Transform = currentTransform * sf.Transform;
						loadingQueue.push(newFile);
						break;
					}
					BV_LOG_ERROR("Couldn't find file {}", fileData.FilePath.string());
				}
				case LDrawLineType::Comment:
					if (reader.isCurrentLineCommand())
					{
						LDrawCommandData commandData = reader.getLineData<LDrawCommandData>();
						LDrawCommandManager::executeCommand(commandData.Extension, commandData.Arguments, settings);
						break;
					}
#if 0
					BV_LOG_INFO("LDraw file comment: {}", reader.getComment());
#endif
					break;
				default:
					break;
			}
		}

		return true;
	}

	void LegoMeshLoader::convertToDm(Ref<Mesh> mesh)
	{
		// Convert LDU to mm
		// 1 LDU = 0.4 mm
		mesh->scale(0.4);
		// Convert mm to dm
		// 1 mm = 0.01 dm;
		mesh->scale(0.01);
	}

}