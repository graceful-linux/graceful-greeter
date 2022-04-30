//
// Created by dingjing on 4/2/22.
//
#include "security-user.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>

#define     GRACEFUL_GREETER_SECURITY_USER_CONFIG_PATH          "/etc/graceful-greeter/graceful-greeter-security-user-config.json"

SecurityUser* SecurityUser::gInstance = nullptr;

SecurityUser::SecurityUser () : mMode (NORMAL)
{
    loadJsonFile ();
}

int SecurityUser::getMode () const
{
    return mMode;
}

QStringList SecurityUser::getBlackList ()
{
    return mBlackList;
}

QStringList SecurityUser::getWhiteList ()
{
    return mWhiteList;
}

SecurityUser *SecurityUser::instance ()
{
    if (!gInstance) {
        gInstance = new SecurityUser;
    }

    return gInstance;
}

bool SecurityUser::isSecurityUser (const QString& username)
{
    if (username == "*login") {
        return true;
    }

    if (username == "*guest") {
        return true;
    }

    switch (mMode) {
        case NORMAL:
            return true;
            break;
        case BLACKLIST:
            if (mBlackList.contains(username))
                return false;
            else
                return true;
        case WHITELIST:
            if (mWhiteList.contains(username))
                return true;
            else
                return false;
        default:
            break;
    }

    return true;
}

void SecurityUser::loadJsonFile ()
{
    QFile file (GRACEFUL_GREETER_SECURITY_USER_CONFIG_PATH);

    if (!file.exists()) {
        return;
    }

    file.open (QIODevice::ReadOnly);
    QString value = file.readAll ();
    file.close();

    QJsonParseError parseJsonErr {};
    QJsonDocument document  = QJsonDocument::fromJson (value.toUtf8(),&parseJsonErr);
    if (QJsonParseError::NoError != parseJsonErr.error) {
        return ;
    }

    QJsonObject rootObj = document.object ();
    if (rootObj.contains (QStringLiteral("graceful-greeter"))){
        QJsonObject greeterObj = rootObj.value (QStringLiteral("graceful-greeter")).toObject();
        if (greeterObj.contains (QStringLiteral("mode"))){
            QString tmp;
            tmp = greeterObj.value (QStringLiteral("mode")).toString();
            if (tmp == "normal") {
                mMode = NORMAL;
            } else if (tmp == "blacklist") {
                mMode = BLACKLIST;
            } else if (tmp == "whitelist") {
                mMode = WHITELIST;
            }
        }

        if(NORMAL == mMode) {
            return;
        }

        if (greeterObj.contains (QStringLiteral("blacklist"))) {
            QJsonArray tmpBlacklist = greeterObj.value (QStringLiteral("blacklist")).toArray ();
            for (auto i : tmpBlacklist) {
                mBlackList.append(i.toString());
            }
        }

        if (greeterObj.contains(QStringLiteral("whitelist"))) {
            QJsonArray tmpWhiteList = greeterObj.value(QStringLiteral("whitelist")).toArray();
            for (auto i : tmpWhiteList){
                mWhiteList.append(i.toString());
            }
        }
    }
}
