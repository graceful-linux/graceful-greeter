//
// Created by dingjing on 4/16/22.
//

#include "greeter-auth.h"

#include <QtCore/QDebug>
#include <QtCore/QObject>

#include <lightdm.h>

class GreeterAuthPrivate : public QObject
{
    Q_OBJECT
public:
    explicit GreeterAuthPrivate (QObject* obj = nullptr, GreeterAuth* parent = nullptr);
    ~GreeterAuthPrivate() override;
    bool isAuthed () const;

public:
    LightDMGreeter             *mLightDMGreeter;

protected:
    /**
     * @brief
     *  The ::idle signal gets emitted when the user has logged in and the greeter is no longer needed.
     *  This signal only matters if the greeter has marked itself as resettable using lightdm_greeter_set_resettable().
     */
    static void idle (LightDMGreeter *greeter, gpointer data);

    /**
     * @brief
     *  The ::reset signal gets emitted when the user is returning to a greeter that was previously marked idle.
     *  This signal only matters if the greeter has marked itself as resettable using lightdm_greeter_set_resettable().
     */
    static void reset (LightDMGreeter *greeter, gpointer data);

    /**
     * @brief
     *  The timed-login signal gets emitted when the automatic login timer has expired.
     *  The application should then call lightdm_greeter_authenticate_autologin().
     */
    static void autoLoginExpired (LightDMGreeter *greeter, gpointer data);

    /**
     * @brief
     *  The authentication-complete signal gets emitted when the greeter has completed authentication.
     * @see void user_function (LightDMGreeter *greeter, gpointer user_data)
     */
    static void authenticationComplete (LightDMGreeter *greeter, gpointer data);

    /**
     * @brief
     *  The ::show-prompt signal gets emitted when the greeter should show a prompt to the user.
     *  The given text should be displayed and an input field for the user to provide a response.
     *
     *  Call lightdm_greeter_respond() with the resultant input or lightdm_greeter_cancel_authentication() to abort the authentication.
     */
    static void showPrompt (LightDMGreeter *greeter, const gchar *text, LightDMPromptType type, gpointer data);

    /**
     * @brief
     *  The ::show-message signal gets emitted when the greeter should show a message to the user.
     *  void user_function (LightDMGreeter* greeter, gchar* text, LightDMMessageType type, gpointer user_data)
     */
    static void showMessage (LightDMGreeter *greeter, const gchar *text, LightDMMessageType type, gpointer data);

public:
    GreeterAuth                *q_ptr;

private:
    Q_DECLARE_PUBLIC(GreeterAuth);
};

#include "greeter-auth.moc"


GreeterAuthPrivate::GreeterAuthPrivate (QObject* obj, GreeterAuth *parent) :
    q_ptr (parent)
{
#if !defined(GLIB_VERSION_2_36)
    g_type_init();
#endif

    g_autoptr(GError) error = nullptr;
    mLightDMGreeter = lightdm_greeter_new();

    lightdm_greeter_set_resettable (mLightDMGreeter, true);

    g_signal_connect (mLightDMGreeter, LIGHTDM_GREETER_SIGNAL_IDLE, G_CALLBACK (idle), this);
    g_signal_connect (mLightDMGreeter, LIGHTDM_GREETER_SIGNAL_RESET, G_CALLBACK (reset), this);
    g_signal_connect (mLightDMGreeter, LIGHTDM_GREETER_SIGNAL_SHOW_PROMPT, G_CALLBACK (showPrompt), this);
    g_signal_connect (mLightDMGreeter, LIGHTDM_GREETER_SIGNAL_SHOW_MESSAGE, G_CALLBACK (showMessage), this);
    g_signal_connect (mLightDMGreeter, LIGHTDM_GREETER_SIGNAL_AUTOLOGIN_TIMER_EXPIRED, G_CALLBACK (autoLoginExpired), this);
    g_signal_connect (mLightDMGreeter, LIGHTDM_GREETER_SIGNAL_AUTHENTICATION_COMPLETE, G_CALLBACK (authenticationComplete), this);

    lightdm_greeter_connect_to_daemon_sync (mLightDMGreeter, &error);
    if (error) {
        qCritical() << "connect to lightDM error, msg: " << error->message;
    }

    q_func()->setSession(q_func()->getDefaultSession());

#if DEBUG
    qDebug() << "default session: " << q_func()->getSession();
#endif

    connect (q_func(), &AuthenticationInterface::cancelAuthentication, this,[=] () {
        g_autoptr(GError) error = nullptr;

        bool ret = lightdm_greeter_cancel_authentication (mLightDMGreeter, &error);
#if DEBUG
        qDebug() << "cancel authentication " << (ret ? "'ok'" : QString("'error', msg: %1").arg (error->message));
#endif
        return ret;
    });
}

void GreeterAuthPrivate::showPrompt(LightDMGreeter *greeter, const gchar *text, LightDMPromptType type, gpointer data)
{
    auto that = static_cast<GreeterAuthPrivate*>(data);
    QString message = QString::fromUtf8(text);

    Q_UNUSED(text)
    Q_UNUSED(type)
    Q_UNUSED(data)
    Q_UNUSED(that)
    Q_UNUSED(greeter);

#if DEBUG
    qDebug() << "lightDM showMessage " << (type == LIGHTDM_PROMPT_TYPE_QUESTION ? "[ QUESTION ]" : "[ SECRET ]") << message;
#endif

    if (LIGHTDM_PROMPT_TYPE_SECRET == type) {
        QString passwd = that->q_func()->getPasswd();
        if (passwd.isNull() || passwd.isEmpty() || passwd == "") {
            Q_EMIT that->q_func()->requestPassword();
        } else {
            g_autoptr(GError) error = nullptr;
#if DEBUG
            qDebug() << "send passwd to lightDM: " << passwd;
#endif
            lightdm_greeter_respond (that->mLightDMGreeter, passwd.toLocal8Bit().constData(), &error);
            if (error) {
                qCritical() << "user " << that->q_func()->getUser() << "'s password is wrong!";
                Q_EMIT that->q_func()->authFailed();
            } 
        }
    }
}

void GreeterAuthPrivate::showMessage(LightDMGreeter *greeter, const gchar *text, LightDMMessageType type, gpointer data)
{
    auto that = static_cast<GreeterAuthPrivate*>(data);
    QString message = QString::fromUtf8(text);

#if DEBUG
    qDebug() << "lightDM showMessage " << (type == LIGHTDM_MESSAGE_TYPE_INFO ? "[ INFO ]" : "[ERROR]") << message;
#endif

    Q_UNUSED(text)
    Q_UNUSED(type)
    Q_UNUSED(data)
    Q_UNUSED(that)
    Q_UNUSED(greeter);


    if (LIGHTDM_MESSAGE_TYPE_ERROR == type) {
        Q_EMIT that->q_ptr->showMessage (message);
    }
}

void GreeterAuthPrivate::authenticationComplete(LightDMGreeter *greeter, gpointer data)
{
    auto that = static_cast<GreeterAuthPrivate*>(data);

#if DEBUG
    qDebug() << "authenticationComplete is successful? " << that->isAuthed();
#endif

    Q_UNUSED(data)
    Q_UNUSED(that)
    Q_UNUSED(greeter);

    if (that->isAuthed()) {
        Q_EMIT that->q_func()->authSuccess();
    } else {
        Q_EMIT that->q_func()->authFailed();
    }
}

void GreeterAuthPrivate::autoLoginExpired(LightDMGreeter *greeter, gpointer data)
{
    auto that = static_cast<GreeterAuthPrivate*>(data);

#if DEBUG
    qDebug() << "cb_autoLoginExpired";
#endif

    Q_UNUSED(data)
    Q_UNUSED(that)
    Q_UNUSED(greeter);
//    Q_EMIT that->q_func()->autologinTimerExpired();
}

void GreeterAuthPrivate::idle(LightDMGreeter *greeter, gpointer data)
{
    auto that = static_cast<GreeterAuthPrivate*>(data);
//    Q_EMIT that->q_func()->idle();

    Q_UNUSED(data)
    Q_UNUSED(that)
    Q_UNUSED(greeter);
}

void GreeterAuthPrivate::reset(LightDMGreeter *greeter, gpointer data)
{
    auto that = static_cast<GreeterAuthPrivate*>(data);

#if DEBUG
    qDebug() << "cb_reset";
#endif

    Q_UNUSED(data)
    Q_UNUSED(that)
    Q_UNUSED(greeter);
//    Q_EMIT that->q_func()->reset();
}

GreeterAuthPrivate::~GreeterAuthPrivate ()
{
    if (mLightDMGreeter) g_object_unref (mLightDMGreeter);
}

bool GreeterAuthPrivate::isAuthed () const
{
    return lightdm_greeter_get_is_authenticated(mLightDMGreeter);
}

GreeterAuth::GreeterAuth (AuthenticationInterface *parent) :
    AuthenticationInterface (parent),
    d_ptr (new GreeterAuthPrivate(nullptr, this))
{
}

GreeterAuth::~GreeterAuth ()
{
    delete d_ptr;
}

void GreeterAuth::authentication ()
{
    Q_D(GreeterAuth);

    g_autoptr(GError) error = nullptr;

    QString user = getUser();
    QString passwd = getPasswd();

    // authentication but not launch session, reset and authentication again
    if (lightdm_greeter_get_in_authentication (d->mLightDMGreeter)) {
        lightdm_greeter_cancel_authentication (d->mLightDMGreeter, &error);
        if (error) {
            qCritical() << "user: '" << user << "' is in authentication, cancel error, msg: " << error->message;
        }
    }


    // FIXME:// lightdm_greeter_authenticate_as_guest
    // FIXME:// lightdm_greeter_authenticate_remote
    lightdm_greeter_authenticate (d->mLightDMGreeter, user.toLocal8Bit().constData(), &error);
    if (error) {
        qCritical() << "set user: '" << user << "' error, msg: " << error->message;
    }
}

QString GreeterAuth::getAuthType ()
{
    return "greeter";
}

bool GreeterAuth::isAuthenticated ()
{
    Q_D(GreeterAuth);

    return d->isAuthed();
}

void GreeterAuth::setUser (const QString &user)
{
    AuthenticationInterface::setUser(user);
}

void GreeterAuth::setPasswd (const QString &passwd)
{
    AuthenticationInterface::setPasswd (passwd);
}

void GreeterAuth::setSession (const QString &session)
{
    AuthenticationInterface::setSession (session);
}

QString GreeterAuth::getDefaultSession ()
{
    GList* ls = lightdm_get_sessions();

    int lsCount = g_list_length (ls);

    if (lsCount <= 0) {
        return "";
    }

    auto ss = static_cast<LightDMSession*>(ls->data);

    QString mSession = QString::fromUtf8(lightdm_session_get_key(ss));

    return mSession;
}

bool GreeterAuth::startSession ()
{
    Q_D(GreeterAuth);

    g_autoptr(GError) error = nullptr;
    QString session = getSession();

#if DEBUG
    qDebug() << "start Session: " << session << "  is active: " << isAuthenticated();
#endif

    lightdm_greeter_start_session_sync(d->mLightDMGreeter, session.toUtf8().constData(), &error);
    if (error) {
        qCritical() << "start session error: " << error->message;
        Q_EMIT showMessage(tr("Session startup error, please select another session."));
        return false;
    }

    return true;
}


