set(src_dir ${CMAKE_CURRENT_SOURCE_DIR}/Src)

set(sources)
set(headers)

# LDraw
set(ldraw_dir ${src_dir}/LDraw)
list(APPEND sources
    ${ldraw_dir}/LDrawCommandManager.cpp
    ${ldraw_dir}/LDrawCore.cpp
    ${ldraw_dir}/LDrawFileManager.cpp
    ${ldraw_dir}/LDrawReader.cpp)

list(APPEND headers
    ${ldraw_dir}/LDrawCommandManager.h
    ${ldraw_dir}/LDrawCore.h
    ${ldraw_dir}/LDrawFileManager.h
    ${ldraw_dir}/LDrawReader.h)

# Lego
set(lego_dir ${src_dir}/Lego)
list(APPEND sources
    ${lego_dir}/LegoMeshReader.cpp
    ${lego_dir}/LegoPartMeshRegistry.cpp)

list(APPEND headers
    ${lego_dir}/LegoMaterial.h
    ${lego_dir}/LegoMeshReader.h
    ${lego_dir}/LegoPart.h
    ${lego_dir}/LegoPartMeshRegistry.h)

# Scene
set(scene_dir ${src_dir}/Scene)
list(APPEND sources
    ${scene_dir}/BrickviewScene.cpp)

list(APPEND headers
    ${scene_dir}/BrickviewComponents.h
    ${scene_dir}/BrickviewScene.h)

add_library(Brickview ${sources} ${headers})

set_target_properties(Brickview PROPERTIES
    CXX_STANDARD 20)

target_include_directories(Brickview PUBLIC ${src_dir})
target_link_libraries(Brickview PUBLIC Soup)
