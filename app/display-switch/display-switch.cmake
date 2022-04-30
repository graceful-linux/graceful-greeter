FILE(GLOB GRACEFUL_GREETER_DISPLAY_SWITCH
        ${CMAKE_SOURCE_DIR}/app/display-switch/display-service.cpp ${CMAKE_SOURCE_DIR}/app/display-switch/display-service.h
        ${CMAKE_SOURCE_DIR}/app/display-switch/display-switch.cpp ${CMAKE_SOURCE_DIR}/app/display-switch/display-switch.h
        ${CMAKE_SOURCE_DIR}/app/display-switch/ui_mainwindow.h
        )

set(GRACEFUL_GREETER_DISPLAY_SWITCH_RESOURCE_FILES ${CMAKE_SOURCE_DIR}/display-switch/ds-res.qrc)