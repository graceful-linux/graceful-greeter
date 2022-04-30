project(demo)

add_executable(user-model-demo
        ${CMAKE_SOURCE_DIR}/app/model/users-model.cpp
        ${CMAKE_SOURCE_DIR}/app/greeter/security-user.cpp
        ${CMAKE_SOURCE_DIR}/demo/user-model-demo.cpp)
target_link_libraries(user-model-demo Qt5::Core Qt5::Widgets ${LIGHTDM_LIBRARIES} ${LIBGLIB_LIBRARIES})
target_include_directories(user-model-demo BEFORE PUBLIC ${LIBGLIB_INCLUDE_DIRS} ${LIGHTDM_INCLUDE_DIRS})

add_executable(session-model-demo
        session-model-demo.cpp
        ${CMAKE_SOURCE_DIR}/app/model/sessions-model.cpp)
target_link_libraries(session-model-demo Qt5::Core Qt5::Widgets ${LIGHTDM_LIBRARIES} ${LIBGLIB_LIBRARIES})
target_include_directories(session-model-demo BEFORE PUBLIC ${LIBGLIB_INCLUDE_DIRS} ${LIGHTDM_INCLUDE_DIRS})

add_executable(greeter-demo
        greeter-demo.cpp
        ${CMAKE_SOURCE_DIR}/app/authentication/greeter-auth.cpp
        ${CMAKE_SOURCE_DIR}/app/authentication/authentication-interface.cpp
        )
target_link_libraries(greeter-demo Qt5::Core Qt5::Widgets ${LIGHTDM_LIBRARIES} ${LIBGLIB_LIBRARIES})
target_include_directories(greeter-demo BEFORE PUBLIC ${LIBGLIB_INCLUDE_DIRS} ${LIGHTDM_INCLUDE_DIRS})

include(${CMAKE_SOURCE_DIR}/app/resource/resource.cmake)
add_executable(icon-edit-demo
        icon-edit-demo.cpp
        ${CMAKE_SOURCE_DIR}/app/ui/widget/icon-edit.h
        ${CMAKE_SOURCE_DIR}/app/ui/widget/icon-edit.cpp

        ${CMAKE_SOURCE_DIR}/app/common/xevent-monitor.h
        ${CMAKE_SOURCE_DIR}/app/common/xevent-monitor.cpp

        ${GRACEFUL_GREETER_RESOURCES}
)
target_link_libraries(icon-edit-demo Qt5::Core Qt5::Widgets ${LIGHTDM_LIBRARIES} ${LIBGLIB_LIBRARIES} ${QT5BASE_LIBRARIES} ${X11_LIBRARIES})
target_include_directories(icon-edit-demo BEFORE PUBLIC ${LIBGLIB_INCLUDE_DIRS} ${LIGHTDM_INCLUDE_DIRS} ${QT5BASE_INCLUDE_DIRS} ${X11_INCLUDE_DIRS})


