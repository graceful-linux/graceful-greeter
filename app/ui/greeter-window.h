//
// Created by dingjing on 4/4/22.
//

#ifndef GRACEFUL_GREETER_GREETER_WINDOW_H
#define GRACEFUL_GREETER_GREETER_WINDOW_H
#include <QLabel>
#include <QTimer>
#include <QLocale>
#include <QWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QTranslator>
#include <QStackedLayout>
#include <QSharedPointer>
#include <QtConcurrent/QtConcurrentRun>

#include "../model/users-model.h"

class QProcess;
class Language;
class UserWindow;
class LoginWindow;
class Configuration;

class Greeter;
class PowerManager;
class SessionsModel;
class SessionWindow;
class AuthenticationInterface;

/**
 * @brief
 *  登录时候验证信息填写的窗口
 */
class GreeterWindow : public QWidget
{
    Q_OBJECT
public:
    explicit GreeterWindow (QWidget *parent = nullptr);
    ~GreeterWindow() override;

    void setRootWindow ();

protected:
    void resizeEvent (QResizeEvent* event) override;
    void keyReleaseEvent (QKeyEvent* event) override;
    bool eventFilter (QObject* obj, QEvent* event) override;

private Q_SLOTS:
    void timedAutologin ();
    void onAuthenticationComplete ();

    void showUserWnd ();
    void showPowerWnd ();
    void showSessionWnd ();

    void onUserChanged (const QString& user);
    void onSessionChanged (const QString& session);

    void onUserSelected (const QModelIndex&);
    void onCurrentUserChanged (const QModelIndex&);
    void onUserChangedByManual (const QString &userName);
    void onLanguageChanged (const QString& languageCode);

private:
    typedef enum
    {
        NONE_MENU,
        USERS_MENU,
        SESSIONS_MENU,
        POWER_MENU,
    } MenuType;

private:
    void switchWindow (MenuType menuType);

    void updateSession (QString userName);
    void updateLanguage (QString userName);
    QString getAccountBackground (uid_t uid);
    bool sessionIsValid (const QString &session);

private:
    UserWindow                         *mUserWnd;
    LoginWindow                        *mLoginWnd;
    SessionWindow                      *mSessionWnd;
    PowerManager                       *mPowerWnd;

    QPushButton                        *mKeyboardBtn{};

    QPushButton                        *mPowerBtn{};

    QPushButton                        *mSwitchUserBtn{};

    QPushButton                        *mSessionBtn{};

    QLayout                            *mWidgetLayout = nullptr;

    UsersModel                         *mUsersModel = nullptr;
    SessionsModel                      *mSessionsModel = nullptr;
    Configuration                      *mConfiguration = nullptr;

    QTimer 		                       *mTimer = nullptr;
    QWidget	    	                   *mWidgetTime = nullptr;
    QLabel		                       *mLBLDate = nullptr;
    QLabel		                       *mLBLTime = nullptr;
    QLocale                             mLocal;

    // authentication, this maybe a map for more than one authentication method.
    AuthenticationInterface*            mAuth;
};
#endif //GRACEFUL_GREETER_GREETER_WINDOW_H
