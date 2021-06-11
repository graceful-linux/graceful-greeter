#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

namespace graceful {

class Cache : public QSettings
{
public:
    static const QString GREETER_DATA_DIR_PATH;
    static void prepare();

    Cache();
    QString getLastUser();
    void setLastUser(QString userId);
    QString getLastSession(QString userId);
    void setLastSession(QString userId, QString session);
};

#define CONFIG_FILE "/etc/lightdm/graceful-greeter.conf"
#define BACKGROUND_IMAGE_KEY "greeter-background-image"
#define LOGINFORM_OFFSETX_KEY "loginform-offset-x"
#define LOGINFORM_OFFSETY_KEY "loginform-offset-y"


class Settings : public QSettings
{
public:
    Settings();
    QString iconThemeName();
    QString backgrundImagePath();

    QString offsetX();
    QString offsetY();
};
}

#endif // SETTINGS_H
