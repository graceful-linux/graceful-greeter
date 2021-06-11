#include <QDebug>
#include <QDir>
#include "settings.h"

#define BACKGROUND_IMAGE_KEY "greeter-background-image"
#define LOGINFORM_OFFSETX_KEY "loginform-offset-x"
#define LOGINFORM_OFFSETY_KEY "loginform-offset-y"
#define LOGFILE_PATH_KEY "logfile-path"


using namespace graceful;

const QString Cache::GREETER_DATA_DIR_PATH = "/var/lib/lightdm/graceful-greeter";

void Cache::prepare()
{
    QDir dir(GREETER_DATA_DIR_PATH);
    if (!dir.exists()) {
        if (!dir.mkpath(GREETER_DATA_DIR_PATH)) {
            qWarning() << "Unable to create dir" << GREETER_DATA_DIR_PATH;
        }
    }
}

Cache::Cache() : QSettings(GREETER_DATA_DIR_PATH + "/state", QSettings::NativeFormat)
{

}

QString Cache::getLastUser()
{
    return value("last-user").toString();
}

void Cache::setLastUser(QString userId)
{
    setValue("last-user", userId);
}

QString Cache::getLastSession(QString userId)
{
    return value(userId + "/last-session").toString();
}

void Cache::setLastSession(QString userId, QString session)
{
    setValue(userId + "/last-session", session);
}


Settings::Settings() : QSettings(QString("/etc/lightdm/graceful-greeter.conf"), QSettings::NativeFormat)
{

}

QString Settings::iconThemeName()
{
    return value("greeter-icon-theme").toString();
}

QString Settings::backgrundImagePath()
{
    return value("greeter-background-image").toString();
}

QString Settings::offsetX()
{
    return value("loginform-offset-x").toString();
}

QString Settings::offsetY()
{
    return value("loginform-offset-y").toString();
}
