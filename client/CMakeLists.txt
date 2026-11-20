project(sampvoice-client)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE sampvoice_client_source_list "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE sampvoice_client_resources_list "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.*")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include $ENV{DXSDK_DIR}/Include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries $ENV{DXSDK_DIR}/Lib/x86)

add_library(${PROJECT_NAME} SHARED ${sampvoice_client_source_list})

add_definitions(-DSAMP_EN -DSAMP_R3)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "sampvoice" SUFFIX ".asi")

set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/resources/gauss.hlsl" PROPERTIES
    HEADER_FILE_ONLY TRUE
    VS_TOOL_OVERRIDE "None"
)

target_sources(${PROJECT_NAME} PRIVATE ${sampvoice_client_resources_list})

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")

target_link_libraries(${PROJECT_NAME} PRIVATE
    bass_fx.lib 
    bass.lib 
    opus.lib
)

GroupSourcesByFolder(${PROJECT_NAME})
