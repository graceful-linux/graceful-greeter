project(app)

include(${CMAKE_SOURCE_DIR}/app/ui/ui.cmake)
include(${CMAKE_SOURCE_DIR}/app/model/model.cmake)
include(${CMAKE_SOURCE_DIR}/app/common/common.cmake)
include(${CMAKE_SOURCE_DIR}/app/resource/resource.cmake)
include(${CMAKE_SOURCE_DIR}/app/display-switch/display-switch.cmake)
include(${CMAKE_SOURCE_DIR}/app/authentication/authentication.cmake)

#add_subdirectory(resource)
add_subdirectory(greeter)


