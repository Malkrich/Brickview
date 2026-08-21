#include "LegoMeshReader.h"

#include "LDraw/LDrawCore.h"
#include "LDraw/LDrawFileManager.h"
#include "LDraw/LDrawReader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Brickview
{

  struct LoadingQueueFileData
  {
    LoadingQueueFileData() = default;
    LoadingQueueFileData(const std::filesystem::path& filePath, const glm::mat4& transform)
      : FilePath(filePath)
      , Transform(transform)
    {
    }

    std::filesystem::path FilePath = "";
    glm::mat4 Transform = glm::mat4(1.0f);
    bool Inverted = false;
  };

  LegoMeshReader::LegoMeshReader()
  {
    LDrawCore::Init();

    // Convert LDU to mm
    // 1 LDU = 0.4 mm
    // Convert mm to dm
    // 1 mm = 0.001 m;
    LDUToMeterTransform = glm::scale(glm::mat4(1.0f), glm::vec3(0.4f * 0.001f));
    // Up direction from LDraw is negative: https://www.ldraw.org/article/218.html (section The LDraw Co-ordinate
    // System)
    glm::mat4 lDrawToBrickviewSpaceTransform(1.0f);
    lDrawToBrickviewSpaceTransform[1] = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
    LDrawToBrickviewSpaceTransform = lDrawToBrickviewSpaceTransform;
  }

  LegoMeshReader::~LegoMeshReader()
  {
    LDrawCore::Shutdown();
  }

  bool LegoMeshReader::Read(const std::filesystem::path& filePath)
  {
    if (!std::filesystem::exists(filePath))
    {
      SP_LOG_ERROR("Couldn't load: {}", filePath.generic_string());
      return false;
    }

    m_Mesh = Soup::CreateRef<Soup::Mesh>();
    std::queue<LoadingQueueFileData> loadingQueue;
    LoadingQueueFileData initialFile(filePath, glm::mat4(1.0f));
    loadingQueue.push(initialFile);

    while (!loadingQueue.empty())
    {
      const LoadingQueueFileData& file = loadingQueue.front();
      ReadFile(file, m_Mesh, loadingQueue);
      loadingQueue.pop();
    }

    // Convert to meter
    m_Mesh->Transform(LDUToMeterTransform);
    // Convert LDraw space to Brickview space
    m_Mesh->Transform(LDrawToBrickviewSpaceTransform);

    return true;
  }

  bool LegoMeshReader::ReadFile(
    const LoadingQueueFileData& file, Soup::Ref<Soup::Mesh> mesh, std::queue<LoadingQueueFileData>& loadingQueue)
  {
    std::filesystem::path currentFilePath = file.FilePath;
    const glm::mat4& currentTransform = file.Transform;

    LDrawReader reader(currentFilePath, file.Inverted);

    if (!reader.IsValid())
    {
      SP_LOG_ERROR("Couldn't read file in {}", currentFilePath.generic_string());
      return false;
    }

    while (reader.ReadLine())
    {
      LDrawLineType lineType = reader.GetLineType();

      switch (lineType)
      {
        case LDrawLineType::Triangle:
        {
          LDrawTriangleData t = reader.GetLineData<LDrawTriangleData>();
          auto winding = reader.GetCurrentWindingState();
          if (winding == LDrawGeometryWinding::CW)
            mesh->AddTriangle(t.p0, t.p1, t.p2, currentTransform);
          else if (winding == LDrawGeometryWinding::CCW)
            mesh->AddTriangle(t.p2, t.p1, t.p0, currentTransform);
          else
            SP_LOG_ERROR("Unknown winding for triangle!");
          break;
        }
        case LDrawLineType::Quadrilateral:
        {
          LDrawQuadData q = reader.GetLineData<LDrawQuadData>();
          auto winding = reader.GetCurrentWindingState();
          if (winding == LDrawGeometryWinding::CW)
            mesh->AddQuad(q.p0, q.p1, q.p2, q.p3, currentTransform);
          else if (winding == LDrawGeometryWinding::CCW)
            mesh->AddQuad(q.p3, q.p2, q.p1, q.p0, currentTransform);
          else
            SP_LOG_ERROR("Unknown winding for quad!");
          break;
        }
        case LDrawLineType::SubFileRef:
        {
          LDrawSubFileRefData sf = reader.GetLineData<LDrawSubFileRefData>();
          const LDrawFileData& fileData = LDrawFileManager::GetFileFromRawFileName(sf.FilePath);

          if (fileData.FileType != LDrawFileType::None)
          {
            LoadingQueueFileData newFile;
            newFile.FilePath = fileData.FilePath;
            newFile.Transform = currentTransform * sf.Transform;
            newFile.Inverted = reader.IsCurrentLineInverted();
            loadingQueue.push(newFile);
            break;
          }
          SP_LOG_ERROR("Couldn't find file {}", fileData.FilePath.string());
        }
#if 0
        case LDrawLineType::Comment:
          SP_LOG_INFO("LDraw file comment: {}", reader.getComment());
          break;
#endif
        default:
          break;
      }
    }

    return true;
  }

}
