//
// Created by dingjing on 4/2/22.
//

#ifndef GRACEFUL_GREETER_SECURITY_USER_H
#define GRACEFUL_GREETER_SECURITY_USER_H
#include <QString>
#include <QStringList>

enum SECURITY_TYPE
{
    NORMAL,
    WHITELIST,
    BLACKLIST
};

class SecurityUser
{
public:
    SecurityUser ();

    int getMode () const;
    QStringList getBlackList ();
    QStringList getWhiteList ();
    static SecurityUser* instance ();
    bool isSecurityUser (const QString& username);

private:
    void loadJsonFile ();

private:
    int                     mMode;
    QStringList             mBlackList;
    QStringList             mWhiteList;

    static SecurityUser    *gInstance;
};


#endif //GRACEFUL_GREETER_SECURITY_USER_H
