TEMPLATE = app
TARGET = graceful-greeter

QT  += core gui widgets dbus x11extras svg
CONFIG += c++11 link_pkgconfig no_keyworlds
PKGCONFIG += liblightdm-qt5-3

prefix = /usr/share/graceful-greeter


DEFINES += \
    RESOURCE_PATH=$${prefix}                                    \
    GREETER_CONFIG=/usr/share/lightdm/graceful-greeter/         \

SOURCES += \
    $$PWD/app/main.cpp                                          \
    $$PWD/app/settings.cpp                                      \
    $$PWD/app/login-form.cpp                                    \
    $$PWD/app/main-window.cpp                                   \

HEADERS += \
    $$PWD/app/settings.h                                        \
    $$PWD/app/login-form.h                                      \
    $$PWD/app/main-window.h                                     \

FORMS += \
    $$PWD/app/login-form.ui

OTHER_FILES += \
    $$PWD/app/graceful-greeter.conf                             \
    $$PWD/app/graceful-greeter.desktop                          \


conf.files += \
    $$PWD/app/graceful-greeter.conf
conf.path = /etc/lightdm

target.path = /usr/local/bin/

desktop.files += \
    $$PWD/app/graceful-greeter.desktop
desktop.path = /usr/share/xgreeters/


INSTALLS += conf target desktop
