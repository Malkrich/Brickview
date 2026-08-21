set(brickview_app_dir ${CMAKE_CURRENT_SOURCE_DIR}/Src)

set(headers
    ${brickview_app_dir}/ApplicationLayer.h

    # Panels
    ${brickview_app_dir}/Panels/LegoPartPropertiesPanel.h
    ${brickview_app_dir}/Panels/LegoPartsExplorerPanel.h
    ${brickview_app_dir}/Panels/ScenePartsListPanel.h)

set(sources
    ${brickview_app_dir}/ApplicationLayer.cpp
    ${brickview_app_dir}/BrickviewApp.cpp

    # Panels
    ${brickview_app_dir}/Panels/LegoPartPropertiesPanel.cpp
    ${brickview_app_dir}/Panels/LegoPartsExplorerPanel.cpp
    ${brickview_app_dir}/Panels/ScenePartsListPanel.cpp)

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
