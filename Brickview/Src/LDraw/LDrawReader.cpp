

#include "LDrawReader.h"

#include "LDrawCommandManager.h"
#include "Utils/FileUtils.h"
#include "Utils/StringUtils.h"

#include <glm/gtc/type_ptr.hpp>

#include <charconv>

namespace Brickview
{
  namespace Utils
  {

    static std::string DeserializeElementAt(const std::string& line, uint32_t index)
    {
      return Soup::StringUtils::GetSubStringAt(line, " ", index);
    }

    // For debug purposes
    static std::string DeserializeComment(const std::string& line)
    {
      size_t beginIndex = Soup::StringUtils::FindNthCharacter(line, ' ', 1);
      return line.substr(beginIndex + 1);
    }

    template <typename T>
    static T DeserializePrimitiveTypeAt(const std::string& line, uint32_t index)
    {
      std::string subStr = DeserializeElementAt(line, index);
      T value;
      std::from_chars(subStr.data(), subStr.data() + subStr.size(), value);
      return value;
    }

    static glm::vec3 DeserializeVector3At(const std::string& line, uint32_t firstElementIndex)
    {
      glm::vec3 v(0.0);
      v.x = DeserializePrimitiveTypeAt<float>(line, firstElementIndex + 0);
      v.y = DeserializePrimitiveTypeAt<float>(line, firstElementIndex + 1);
      v.z = DeserializePrimitiveTypeAt<float>(line, firstElementIndex + 2);
      return v;
    }

    static glm::mat4 DeserializeTransform(const std::string& line, uint32_t index)
    {
      // from: https://www.ldraw.org/article/218.html
      float x, y, z, a, b, c, d, e, f, g, h, i;
      x = DeserializePrimitiveTypeAt<float>(line, index + 0);
      y = DeserializePrimitiveTypeAt<float>(line, index + 1);
      z = DeserializePrimitiveTypeAt<float>(line, index + 2);
      a = DeserializePrimitiveTypeAt<float>(line, index + 3);
      b = DeserializePrimitiveTypeAt<float>(line, index + 4);
      c = DeserializePrimitiveTypeAt<float>(line, index + 5);
      d = DeserializePrimitiveTypeAt<float>(line, index + 6);
      e = DeserializePrimitiveTypeAt<float>(line, index + 7);
      f = DeserializePrimitiveTypeAt<float>(line, index + 8);
      g = DeserializePrimitiveTypeAt<float>(line, index + 9);
      h = DeserializePrimitiveTypeAt<float>(line, index + 10);
      i = DeserializePrimitiveTypeAt<float>(line, index + 11);

      glm::mat4 transform;
      transform[0] = { a, d, g, 0.0f };
      transform[1] = { b, e, h, 0.0f };
      transform[2] = { c, f, i, 0.0f };
      transform[3] = { x, y, z, 1.0f };

      return transform;
    }

  }

  LDrawTriangleData LDrawTriangleData::Deserialize(const std::string& line)
  {
    LDrawTriangleData t;
    t.p0 = Utils::DeserializeVector3At(line, 2);
    t.p1 = Utils::DeserializeVector3At(line, 5);
    t.p2 = Utils::DeserializeVector3At(line, 8);
    return t;
  }

  LDrawQuadData LDrawQuadData::Deserialize(const std::string& line)
  {
    LDrawQuadData q;
    q.p0 = Utils::DeserializeVector3At(line, 2);
    q.p1 = Utils::DeserializeVector3At(line, 5);
    q.p2 = Utils::DeserializeVector3At(line, 8);
    q.p3 = Utils::DeserializeVector3At(line, 11);
    return q;
  }

  LDrawSubFileRefData LDrawSubFileRefData::Deserialize(const std::string& line)
  {
    LDrawSubFileRefData sf;
    sf.Transform = Utils::DeserializeTransform(line, 2);
    std::string filePath = Utils::DeserializeElementAt(line, 14);
#if SP_LINUX
    filePath = Soup::FileUtils::FormatFilePath(filePath, Soup::FileUtils::SeparatorType::ForwardSlash);
#endif
    sf.FilePath = filePath;
    return sf;
  }

  LDrawCommandData LDrawCommandData::Deserialize(const std::string& line)
  {
    LDrawCommandData command;
    std::string rawCommandExtension = Utils::DeserializeElementAt(line, 1);
    command.Extension = LDrawCommandManager::GetCommandExtension(rawCommandExtension);

    uint32_t argIndex = 2;
    bool isCommandArgument = true;
    while (isCommandArgument)
    {
      std::string rawCommandArg = Utils::DeserializeElementAt(line, argIndex);
      isCommandArgument = rawCommandArg != "";

      if (isCommandArgument)
      {
        LDrawCommandArgument arg = LDrawCommandManager::GetCommandArgument(rawCommandArg);
        command.Arguments.push_back(arg);
      }

      argIndex++;
    }

    return command;
  }

  LDrawReader::LDrawReader(const std::filesystem::path& filePath, bool inverted)
    : m_FileInverted(inverted)
  {
    if (!std::filesystem::exists(filePath))
      m_Valid = false;

    m_FileStream = std::ifstream(filePath);
  }

  std::string LDrawReader::LineTypeToString(LDrawLineType type)
  {
    switch (type)
    {
      case LDrawLineType::Empty:
        return "Empty";
      case LDrawLineType::Comment:
        return "Comment";
      case LDrawLineType::SubFileRef:
        return "Sub-file reference";
      case LDrawLineType::Line:
        return "Line";
      case LDrawLineType::Triangle:
        return "Triangle";
      case LDrawLineType::Quadrilateral:
        return "Quadrilateral";
      case LDrawLineType::OptionalFile:
        return "Optional file";
    }

    return "Unkown";
  }

  std::string LDrawReader::GetComment() const
  {
    std::string comment = Utils::DeserializeComment(m_CurrentLine);
    return comment;
  }

  LDrawGeometryWinding LDrawReader::GetCurrentWindingState() const
  {
    LDrawGeometryWinding winding = m_CurrentLineStates.Winding;
    // Not inverted or unknown winding case
    if (!m_CurrentLineStates.Inverted || winding == LDrawGeometryWinding::Unknown)
      return winding;

    // Inverted case
    return winding == LDrawGeometryWinding::CW ? LDrawGeometryWinding::CCW : LDrawGeometryWinding::CW;
  }

  bool LDrawReader::ReadLine()
  {
    std::istream& s = std::getline(m_FileStream, m_CurrentLine);
#ifdef SP_LINUX
    m_CurrentLine.erase(std::remove(m_CurrentLine.begin(), m_CurrentLine.end(), '\r'), m_CurrentLine.end());
#endif

    m_CurrentLineType = DeserializeLineType(m_CurrentLine);

    // Meta Command
    bool currentLineIsCommand = false;
    if (m_CurrentLineType == LDrawLineType::Comment)
    {
      std::string rawCommandExtension = Utils::DeserializeElementAt(m_CurrentLine, 1);
      currentLineIsCommand = LDrawCommandManager::IsCommand(rawCommandExtension);
    }

    // Reset invert next
    if (m_CurrentLineIndex >= m_InvertNextResetLineIndex)
      m_CurrentLineStates.Inverted = m_FileInverted;

    if (currentLineIsCommand)
    {
      LDrawCommandData commandData = GetLineData<LDrawCommandData>();
      LDrawCommandManager::ExecuteCommand(commandData.Extension, commandData.Arguments, m_CurrentLineStates);

      if (m_CurrentLineStates.Inverted != m_FileInverted)
        m_InvertNextResetLineIndex = m_CurrentLineIndex + 2;
    }

    m_CurrentLineIndex++;
    return (bool)s;
  }

  LDrawLineType LDrawReader::DeserializeLineType(const std::string& line)
  {
    if (line.empty())
      return LDrawLineType::Empty;

    LDrawLineType lineType = (LDrawLineType)Utils::DeserializePrimitiveTypeAt<uint32_t>(line, 0);

    return lineType;
  }

}
