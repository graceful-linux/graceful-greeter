project(graceful-greeter)
cmake_minimum_required(VERSION 3.15)

SET(QT_MINIMUM_VERSION "5.12.0")
SET(GLIB_MINIMUM_VERSION "2.50.0")

SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(QT_USE_QTGUI TRUE)
SET(QT_USE_QTXML TRUE)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_BINARY_DIR "/bin")
#SET(CMAKE_CXX_FLAGS "-fPIC")
SET(GRACEFUL_GREETER_DEBUG 0)
SET(CMAKE_INCLUDE_DIRECTORIES_BEFORE ON)

#if (Qt5_POSITION_INDEPENDENT_CODE)
#    SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
#endif()

ADD_DEFINITIONS("-Wall" "-g" "-O0"
        "-DQT_NO_KEYWORDS"
        "-DAPP_NAME=\"graceful-greeter\""
        "-DDEBUG=${GRACEFUL_GREETER_DEBUG}"
        "-DBUILD_DIR=\"${CMAKE_SOURCE_DIR}\""
        "-DGREETER_CONFIG=\"/usr/share/lightdm/graceful-greeter/\"")

find_package(PkgConfig)
find_package(Qt5Core "${QT_MINIMUM_VERSION}" REQUIRED)
find_package(Qt5Widgets "${QT_MINIMUM_VERSION}" REQUIRED)
find_package(Qt5X11Extras "${QT_MINIMUM_VERSION}" REQUIRED)
#find_package(Qt5LinguistTools "${QT_MINIMUM_VERSION}" REQUIRED)

pkg_check_modules (X11 REQUIRED x11 xrandr xtst imlib2)
pkg_check_modules (LIBGLIB REQUIRED glib-2.0 gio-2.0)
pkg_check_modules (QT5BASE REQUIRED Qt5Core Qt5Widgets Qt5Svg Qt5X11Extras Qt5DBus)
pkg_check_modules (LIGHTDM REQUIRED liblightdm-gobject-1)

add_subdirectory(app)
add_subdirectory(demo)
add_subdirectory(data)


# 调用预编译器moc，需要使用 QT5_WRAP_CPP宏
#QT5_WRAP_CPP(ALL_HEADERS_MOC ${ALL_HEADERS})

# 使用rcc处理.qrc文件
#QT5_ADD_RESOURCES(ALL_RESOURCES_RCC ${ALL_QRC})

option(BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)
if(BUILD_DOCUMENTATION)
    find_package(Doxygen REQUIRED)
    configure_file("${CMAKE_SOURCE_DIR}/Doxyfile" @ONLY)
    add_custom_target(doc ALL
            ${DOXYGEN_EXECUTABLE} "${CMAKE_SOURCE_DIR}/Doxyfile"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "Generating API documentation with Doxygen" VERBATIM
            )

    MESSAGE("generate doc in ${CMAKE_BINARY_DIR}/doc/")
#    install(DIRECTORY
#            "${CMAKE_SOURCE_DIR}/doc/html"
#            DESTINATION /usr/share/
#            COMPONENT Devel
#            )
endif()
