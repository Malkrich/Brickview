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

		std::queue<LoadingQueueFileData> loadingQueue;
		LoadingQueueFileData initialFile(filePath, LDrawFileType::Part, glm::mat4(1.0f));
		loadingQueue.push(initialFile);

		while (!loadingQueue.empty())
		{
			const LoadingQueueFileData& file = loadingQueue.front();
			readFile(file, mesh, loadingQueue);
			loadingQueue.pop();
		}

		// TODO: switch all points y coord (LDraw is -y vertical)
		convertToDm(mesh);

		return true;
	}

	bool LegoMeshLoader::readFile(const LoadingQueueFileData& file, Ref<Mesh> mesh, std::queue<LoadingQueueFileData>& loadingQueue)
	{
		std::filesystem::path currentFilePath = file.FilePath;
		const glm::mat4& currentTransform     = file.Transform;

		LDrawReader reader(currentFilePath, file.Inverted);

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
					auto winding = reader.getCurrentWindingState();
					if (winding == LDrawGeometryWinding::CW)
						mesh->addTriangle(t.p0, t.p1, t.p2, currentTransform);
					else if (winding == LDrawGeometryWinding::CCW)
						mesh->addTriangle(t.p2, t.p1, t.p0, currentTransform);
					else
						BV_LOG_ERROR("Unknown winding for triangle!");
					break;
				}
				case LDrawLineType::Quadrilateral:
				{
					LDrawQuadData q = reader.getLineData<LDrawQuadData>();
					auto winding = reader.getCurrentWindingState();
					if (winding == LDrawGeometryWinding::CW)
						mesh->addQuad(q.p0, q.p1, q.p2, q.p3, currentTransform);
					else if (winding == LDrawGeometryWinding::CCW)
						mesh->addQuad(q.p3, q.p2, q.p1, q.p0, currentTransform);
					else
						BV_LOG_ERROR("Unknown winding for quad!");
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
						newFile.Inverted  = reader.isCurrentLineInverted();
						loadingQueue.push(newFile);
						break;
					}
					BV_LOG_ERROR("Couldn't find file {}", fileData.FilePath.string());
				}
#if 0
				case LDrawLineType::Comment:
					BV_LOG_INFO("LDraw file comment: {}", reader.getComment());
					break;
#endif
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