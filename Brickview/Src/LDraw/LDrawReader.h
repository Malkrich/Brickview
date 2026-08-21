#pragma once

#include <glm/glm.hpp>

namespace Brickview
{

  enum class LDrawLineType
  {
    Empty = -1,
    Comment = 0,
    SubFileRef = 1,
    Line = 2,
    Triangle = 3,
    Quadrilateral = 4,
    OptionalFile = 5,
  };

  enum class LDrawGeometryWinding
  {
    Unknown = 0,
    CW,
    CCW
  };

  struct LDrawTriangleData
  {
    static LDrawTriangleData Deserialize(const std::string& line);

    glm::vec3 p0, p1, p2;
  };

  struct LDrawQuadData
  {
    static LDrawQuadData Deserialize(const std::string& line);

    glm::vec3 p0, p1, p2, p3;
  };

  struct LDrawSubFileRefData
  {
    static LDrawSubFileRefData Deserialize(const std::string& line);

    std::filesystem::path FilePath;
    glm::mat4 Transform;
  };

  enum class LDrawCommandExtension;
  enum class LDrawCommandArgument;
  struct LDrawCommandData
  {
    static LDrawCommandData Deserialize(const std::string& line);

    LDrawCommandExtension Extension;
    std::vector<LDrawCommandArgument> Arguments;
  };

  class LDrawReader
  {
  public:
    struct States
    {
      LDrawGeometryWinding Winding = LDrawGeometryWinding::Unknown;
      bool Inverted = false;
    };

  public:
    LDrawReader(const std::filesystem::path& filePath, bool inverted);

    // For debug purposes
    static std::string LineTypeToString(LDrawLineType type);
    std::string GetComment() const;

    bool ReadLine();
    bool IsValid() const { return m_Valid; }

    LDrawLineType GetLineType() const { return m_CurrentLineType; }
    LDrawGeometryWinding GetCurrentWindingState() const;
    bool IsCurrentLineInverted() const { return m_CurrentLineStates.Inverted; }
    template <typename T>
    T GetLineData() const
    {
      T data = T::Deserialize(m_CurrentLine);
      return data;
    }

  private:
    LDrawLineType DeserializeLineType(const std::string& line);

  private:
    // Global file states
    std::ifstream m_FileStream;
    bool m_Valid = true;
    bool m_FileInverted;

    // Current states
    uint32_t m_CurrentLineIndex = 0;
    uint32_t m_InvertNextResetLineIndex = 0;
    std::string m_CurrentLine;
    LDrawLineType m_CurrentLineType = LDrawLineType::Empty;
    States m_CurrentLineStates;
  };

}
