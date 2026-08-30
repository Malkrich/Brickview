set(src_dir ${CMAKE_CURRENT_SOURCE_DIR}/Src)

set(headers)
set(sources)

# Application
list(APPEND sources
    ${src_dir}/ApplicationLayer.cpp
    ${src_dir}/BrickviewApp.cpp)

list(APPEND headers
    ${src_dir}/ApplicationLayer.h)

# Panels
set(panels_dir ${src_dir}/Panels)
list(APPEND sources
    ${panels_dir}/LegoPartPropertiesPanel.cpp
    ${panels_dir}/LegoPartsExplorerPanel.cpp
    ${panels_dir}/ScenePartsCollectionPanel.cpp
    ${panels_dir}/ViewportPanel.cpp)

list(APPEND headers
    ${panels_dir}/LegoPartPropertiesPanel.h
    ${panels_dir}/LegoPartsExplorerPanel.h
    ${panels_dir}/ScenePartsCollectionPanel.h
    ${panels_dir}/ViewportPanel.h)

add_executable(BrickviewApp
    ${headers} ${sources})

target_link_libraries(BrickviewApp PRIVATE Brickview)

set_target_properties(BrickviewApp PROPERTIES
    CXX_STANDARD 20)

if(MSVC)
    set_target_properties(BrickviewApp PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VS_STARTUP_PROJECT BrickviewApp)
endif()
