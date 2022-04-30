//
// Created by dingjing on 4/2/22.
//
#include "configuration.h"

#include <QDir>
#include <QFont>
#include <QPainter>
#include <QTextOption>
#include <QFontMetrics>

#define CONFIG_FILE     "/etc/lightdm/graceful-greeter.conf"

static QString getSystemVersion ();
static QString getSystemDistrib ();
static QPixmap logoGenerator (const QString &text);

Configuration* Configuration::gInstance = nullptr;

Configuration::Configuration (QObject *parent) : QObject (parent), mTrans(nullptr)
{
    QString recordFile = QDir::homePath() + "/.cache/graceful-greeter.conf";

    mConfigSettings = new QSettings (CONFIG_FILE, QSettings::IniFormat, this);
    mRecordSettings = new QSettings (recordFile, QSettings::IniFormat, this);
}

QPixmap Configuration::getLogo ()
{
    QString logoFilePath = getValue("logo").toString();
    if (QFile(logoFilePath).exists()) {
        return QPixmap (logoFilePath);
    }

    return logoGenerator(getSystemVersion());
}

bool Configuration::getLastNumLock ()
{
    mRecordSettings->beginGroup("Greeter");
    if (!mRecordSettings->contains("numlock")) {
        mRecordSettings->setValue("numlock", true);
        mRecordSettings->sync();
        mRecordSettings->endGroup();
        return true;
    }

    bool lastNumLock = mRecordSettings->value("numlock").toBool();
    mRecordSettings->endGroup();

    return lastNumLock;
}

bool Configuration::getUseFirstDevice ()
{
    return false;
}

QString Configuration::getLastLoginUser ()
{
    mRecordSettings->beginGroup("Greeter");
    QString lastLoginUser = mRecordSettings->value("lastLoginUser").toString();
    mRecordSettings->endGroup();

    return lastLoginUser;
}

void Configuration::saveLastNumLock (bool value)
{
    mRecordSettings->beginGroup("Greeter");
    mRecordSettings->setValue("numlock", value);
    mRecordSettings->endGroup();
    mRecordSettings->sync();
}

QString Configuration::getDefaultBackgroundName ()
{
    return ":/images/images/bg.jpg";
}

QVariant Configuration::getValue (const QString& key)
{
    mConfigSettings->beginGroup ("Greeter");

    QVariant val = mConfigSettings->value (key);
    mConfigSettings->endGroup();

    return val;
}

void Configuration::saveLastLoginUser (const QString& userRealName)
{
    mRecordSettings->beginGroup("Greeter");
    mRecordSettings->setValue("lastLoginUser", userRealName);
    mRecordSettings->endGroup();
    mRecordSettings->sync();
}

void Configuration::setValue (const QString& key, const QVariant& value)
{
    mConfigSettings->beginGroup("Greeter");
    mConfigSettings->setValue(key, value);
    mConfigSettings->endGroup();
}

Configuration *Configuration::instance (QObject *parent)
{
    if (!gInstance) {
        gInstance = new Configuration (parent);
    }

    return gInstance;
}


static QString getSystemVersion ()
{
    QSettings settings ("/etc/lsb-release", QSettings::IniFormat);
    QString release = settings.value("DISTRIB_RELEASE").toString();
    QString description = settings.value("DISTRIB_DESCRIPTION").toString();

    if (description.right(3) == "LTS") {
        release = release + " LTS";
    }

    return release;
}

static QString getSystemDistrib ()
{
    QSettings settings("/etc/lsb-release", QSettings::IniFormat);
    QString distribId = settings.value("DISTRIB_ID").toString();

    return distribId;
}

static QPixmap logoGenerator (const QString &text)
{
    QPixmap logoBare;
    logoBare.load(":/images/images/logo.png");

    QFont font;
    font.setPixelSize (18);
    QFontMetrics fm(font);
    int textPixelSize = fm.width(text);

    QPixmap logo(logoBare.width() + textPixelSize + 3 + 30, logoBare.height());
    logo.fill(Qt::transparent);

    QRect logoBareRect(30 , 0, logoBare.width(), logoBare.height());
    QPainter painter;
    painter.begin(&logo);
    painter.drawPixmap(logoBareRect, logoBare, logoBare.rect());

    painter.setPen(Qt::white);
    painter.setFont(font);
    QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
    option.setWrapMode(QTextOption::WordWrap);
    QRect versionRect(logoBareRect.right()+3, 3, logo.width() - logoBare.width(), logo.height());
    painter.drawText(versionRect, text, option);

    return logo;
}
