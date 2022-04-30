//
// Created by dingjing on 4/14/22.
//

#ifndef GRACEFUL_GREETER_AUTHENTICATION_INTERFACE_H
#define GRACEFUL_GREETER_AUTHENTICATION_INTERFACE_H

#include <QtCore/QObject>

class AuthenticationInterfacePrivate;

class AuthenticationInterface : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString mAuthType READ getAuthType)
    Q_PROPERTY(QString mUser READ getUser WRITE setUser)
    Q_PROPERTY(QString mPasswd READ getPasswd WRITE setPasswd)
    Q_PROPERTY(QString mSession READ getSession WRITE setSession)

    Q_PROPERTY(bool mIsAuthed READ isAuthenticated)
public:
    explicit AuthenticationInterface (QObject* parent=nullptr);
    ~AuthenticationInterface() override = default;

    QString getUser ();
    virtual void setUser (const QString& user);

    void clearPasswd ();
    virtual void setPasswd (const QString& passwd);

    QString getSession ();
    virtual void setSession (const QString& session);

    virtual QString getAuthType () = 0;
    virtual bool isAuthenticated () = 0;

    /**
     * @brief
     *  Default session hint.
     *  Owner: LightDMGreeter
     */
    virtual QString getDefaultSession () = 0;

    virtual void authentication () = 0;

    /**
     *
     */
    virtual bool startSession () = 0;

Q_SIGNALS:
    // send to UI
    __attribute__((unused)) void requestUserName ();
    __attribute__((unused)) void requestPassword ();
    __attribute__((unused)) void showMessage (const QString& msg);

    // send to authentication instance
    __attribute__((unused)) void authFailed ();
    __attribute__((unused)) void authSuccess ();
    __attribute__((unused)) bool cancelAuthentication ();

protected:
    QString getPasswd ();

private:
    AuthenticationInterfacePrivate                     *d_ptr;
    Q_DECLARE_PRIVATE(AuthenticationInterface)
};


#endif //GRACEFUL_GREETER_AUTHENTICATION_INTERFACE_H
