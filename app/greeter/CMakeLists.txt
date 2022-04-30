project(greeter)

FILE(GLOB GRACEFUL_GREETER
        screen-model.cpp screen-model.h
        proxy-model.cpp proxy-model.h
        security-user.cpp security-user.h
        users-model.cpp users-model.h
        utils.cpp utils.h
        main-window.cpp main-window.h
        greeter-window.cpp greeter-window.h
        power-manager.cpp power-manager.h
        language.cpp language.h
        language-widget.cpp language-widget.h
        login-window.cpp login-window.h
        pam-tally.cpp pam-tally.h
        session-window.cpp session-window.h
        sessions-model.cpp sessions-model.h
        user-window.cpp user-window.h
        power.cpp power.h
        )

add_executable (graceful-greeter main.cpp
        ${GRACEFUL_GREETER}
        ${GRACEFUL_GREETER_COMMON}
        ${GRACEFUL_GREETER_DISPLAY_SWITCH}
        ${GRACEFUL_GREETER_RESOURCES}
        ${GRACEFUL_GREETER_AUTHENTICATION}
        ${GRACEFUL_GREETER_UI}
        ${GRACEFUL_GREETER_MODEL}
        )
target_link_libraries (graceful-greeter ${QT5BASE_LIBRARIES} ${X11_LIBRARIES} ${LIGHTDM_LIBRARIES} ${LIBGLIB_LIBRARIES})
target_include_directories (graceful-greeter PUBLIC ${QT5BASE_INCLUDE_DIRS} ${X11_INCLUDE_DIRS} ${LIBGLIB_INCLUDE_DIRS} ${LIGHTDM_INCLUDE_DIRS})
set_target_properties (graceful-greeter PROPERTIES OUTPUT_NAME "graceful-greeter")

install(TARGETS graceful-greeter DESTINATION bin)