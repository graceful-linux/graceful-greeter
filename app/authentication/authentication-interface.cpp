//
// Created by dingjing on 4/14/22.
//

#include "authentication-interface.h"

class AuthenticationInterfacePrivate
{
public:
    explicit AuthenticationInterfacePrivate (AuthenticationInterface* parent);

protected:
    AuthenticationInterface        *q_ptr;

    QString                         mUser;
    QString                         mPasswd;
    QString                         mSession;

public:
private:
    Q_DECLARE_PUBLIC(AuthenticationInterface)
};

AuthenticationInterfacePrivate::AuthenticationInterfacePrivate (AuthenticationInterface* parent) :
    q_ptr (parent)
{
    mUser = "";
    mSession = "";
}

AuthenticationInterface::AuthenticationInterface (QObject *parent) :
    QObject (parent),
    d_ptr (new AuthenticationInterfacePrivate(this))
{

}

QString AuthenticationInterface::getUser ()
{
    Q_D(AuthenticationInterface);

    return d->mUser;
}

void AuthenticationInterface::setUser (const QString &user)
{
    Q_D(AuthenticationInterface);

    if (user.isNull() || user.isEmpty() || "" == user) {
        Q_EMIT requestUserName();
        return;
    }

    d->mUser = user;
}

QString AuthenticationInterface::getPasswd ()
{
    Q_D(AuthenticationInterface);

    return d->mPasswd;
}

void AuthenticationInterface::clearPasswd ()
{
    Q_D(AuthenticationInterface);

    d->mPasswd = "";
}

void AuthenticationInterface::setPasswd (const QString &passwd)
{
    Q_D(AuthenticationInterface);

    if (passwd.isNull() || passwd.isEmpty()) {
        Q_EMIT requestPassword();
        return;
    }

    d->mPasswd = passwd;
}

QString AuthenticationInterface::getSession ()
{
    Q_D(AuthenticationInterface);

    if (d->mSession.isNull() || d->mSession.isEmpty() || d->mSession.length() <= 0) {
        d->mSession = getDefaultSession();
    }

    return d->mSession;
}

void AuthenticationInterface::setSession (const QString &session)
{
    if (session.isNull() || session.isEmpty()) {
        return;
    }

    Q_D(AuthenticationInterface);

    if (session.isNull() || session.isEmpty()) {
        return;
    }

    d->mSession = session;
}


