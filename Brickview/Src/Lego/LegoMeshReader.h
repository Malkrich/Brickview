#pragma once

namespace Soup
{
  class Mesh;
}

namespace Brickview
{

  struct LoadingQueueFileData;

  class LegoMeshReader
  {
  public:
    LegoMeshReader();

    Soup::Ref<Soup::Mesh> Read(const std::filesystem::path& filePath);

  private:
    bool ReadFile(
      const LoadingQueueFileData& file, Soup::Ref<Soup::Mesh> mesh, std::queue<LoadingQueueFileData>& loadingQueue);

  private:
    glm::mat4 LDUToMeterTransform;
    glm::mat4 LDrawToBrickviewSpaceTransform;
  };

}
