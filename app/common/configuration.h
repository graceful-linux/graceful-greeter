//
// Created by dingjing on 4/2/22.
//

#ifndef GRACEFUL_GREETER_CONFIGURATION_H
#define GRACEFUL_GREETER_CONFIGURATION_H
#include <QObject>
#include <QPixmap>
#include <QSettings>
#include <QTranslator>

class Configuration : public QObject
{
    Q_OBJECT
private:
    explicit Configuration (QObject *parent = nullptr);

public:
    QPixmap getLogo ();
    bool getLastNumLock ();
    static bool getUseFirstDevice ();
    QString getLastLoginUser ();
    void saveLastNumLock (bool value);
    static QString getDefaultBackgroundName ();
    QVariant getValue (const QString &);
    void saveLastLoginUser (const QString &);
    void setValue (const QString&, const QVariant &);
    static Configuration *instance (QObject *parent = nullptr);

public:
    QTranslator                *mTrans;

private:
    QSettings                  *mConfigSettings;
    QSettings                  *mRecordSettings;

    static Configuration       *gInstance;
};


#endif //GRACEFUL_GREETER_CONFIGURATION_H
