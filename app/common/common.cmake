FILE(GLOB GRACEFUL_GREETER_COMMON
        ${CMAKE_SOURCE_DIR}/app/common/log.h ${CMAKE_SOURCE_DIR}/app/common/log.cpp
        ${CMAKE_SOURCE_DIR}/app/common/global.h ${CMAKE_SOURCE_DIR}/app/common/global.cpp
        ${CMAKE_SOURCE_DIR}/app/common/fake-dialog.h ${CMAKE_SOURCE_DIR}/app/common/fake-dialog.cpp
        ${CMAKE_SOURCE_DIR}/app/common/border-dialog.h ${CMAKE_SOURCE_DIR}/app/common/border-dialog.cpp
        ${CMAKE_SOURCE_DIR}/app/common/configuration.h ${CMAKE_SOURCE_DIR}/app/common/configuration.cpp
        ${CMAKE_SOURCE_DIR}/app/common/monitor-watcher.h ${CMAKE_SOURCE_DIR}/app/common/monitor-watcher.cpp
        ${CMAKE_SOURCE_DIR}/app/common/xevent-monitor.h ${CMAKE_SOURCE_DIR}/app/common/xevent-monitor.cpp
)