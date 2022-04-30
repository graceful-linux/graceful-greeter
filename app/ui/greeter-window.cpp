//
// Created by dingjing on 4/4/22.
//

#include "greeter-window.h"

#include <QMenu>
#include <QRect>
#include <QDebug>
#include <QLayout>
#include <QWindow>
#include <QWidget>
#include <QProcess>
#include <QPainter>
#include <QDateTime>
#include <QScrollBar>
#include <QTranslator>
#include <QResizeEvent>
#include <QApplication>
#include <QStandardPaths>
#include <QtConcurrent/QtConcurrentRun>

#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>

#include "user-window.h"
#include "main-window.h"
#include "login-window.h"
#include "session-window.h"

#include "../model/sessions-model.h"

#include "../greeter/power-manager.h"

#include "../common/global.h"
#include "../common/configuration.h"

#include "../authentication/greeter-auth.h"

#include <pwd.h>
#include <sys/types.h>

#define GREETER_WINDOW_ICON_SIZE            48

GreeterWindow::GreeterWindow (QWidget *parent)
    : QWidget(parent),
    mUserWnd(nullptr),
    mLoginWnd(nullptr),
    mSessionWnd(nullptr),
    mPowerWnd(nullptr),
    mUsersModel(new UsersModel(false)),
    mSessionsModel(new SessionsModel (SessionsModel::LocalSessions)),
    mConfiguration(Configuration::instance()),
    mAuth(nullptr)
{
#if DEBUG
    if (parent) qDebug() << "MainWindow size: " << parent->size() << " greeter window size: " << size();
#endif

    // 时间显示
    mTimer = new QTimer (this);
    mWidgetTime = new QWidget (this);
    mLBLDate = new QLabel (this);
    mLBLTime = new QLabel (this);

    mLocal = QLocale::system ().language ();
    QDateTime dateTime = QDateTime::currentDateTime ();
    QString strFormat = "dd.MM.yyyy, ddd MMMM d yy, hh:mm:ss.zzz, h:m:s ap";
    QString strDateTime = mLocal.toString (dateTime, strFormat);

    mLBLDate->setAlignment (Qt::AlignVCenter | Qt::AlignHCenter);
    mLBLDate->setText (mLocal.toString(QDate::currentDate(),"yyyy/MM/dd ddd"));
    mLBLDate->setStyleSheet ("QLabel{color:white; font-size: 16px;}");

    mLBLTime->setAlignment (Qt::AlignVCenter | Qt::AlignHCenter);
    mLBLTime->setText (mLocal.toString(QDateTime::currentDateTime(),"hh:mm:ss"));
    mLBLTime->setStyleSheet ("QLabel{color:white; font-size: 50px;}");

    mWidgetLayout = new QVBoxLayout (mWidgetTime);
    mWidgetLayout->addWidget (mLBLTime);
    mWidgetLayout->addWidget (mLBLDate);

    mWidgetTime->adjustSize ();

    connect (mTimer, &QTimer::timeout, this, [=] () {
        mLBLTime->setText (mLocal.toString (QDateTime::currentDateTime (), "hh:mm:ss"));
        mLBLDate->setText (mLocal.toString (QDate::currentDate (), "yyyy/MM/dd ddd"));
    });
    mTimer->start(1000);

#if DEBUG
    mWidgetTime->setStyleSheet(QString::fromUtf8 ("border:1px solid blue"));
#endif

    // 电源管理按钮
    mPowerBtn = new QPushButton (this);
    mPowerBtn->setObjectName (QStringLiteral("powerButton"));
    mPowerBtn->setIcon (QPixmap(":/images/images/power.png"));
    mPowerBtn->setIconSize (QSize(30, 30));
    mPowerBtn->setFocusPolicy (Qt::NoFocus);
    mPowerBtn->setFixedSize (GREETER_WINDOW_ICON_SIZE, GREETER_WINDOW_ICON_SIZE);
    mPowerBtn->setCursor (Qt::PointingHandCursor);
    mPowerBtn->installEventFilter (this);
    connect (mPowerBtn, &QPushButton::clicked, this, [=] () {switchWindow(POWER_MENU);});

#if DEBUG
    mPowerBtn->setStyleSheet(QString::fromUtf8 ("border:1px solid blue"));
#endif

    // 会话选择按钮
    mSessionBtn = new QPushButton (this);
    mSessionBtn->setObjectName ((QStringLiteral("sessionButton")));
    mSessionBtn->setIconSize (QSize(24, 24));
    mSessionBtn->setFocusPolicy (Qt::NoFocus);
    mSessionBtn->setFixedSize (GREETER_WINDOW_ICON_SIZE, GREETER_WINDOW_ICON_SIZE);
    mSessionBtn->setCursor (Qt::PointingHandCursor);
    mSessionBtn->installEventFilter (this);
    mSessionBtn->setIcon (QIcon(SessionsModel::getSessionIconPath (mSessionsModel->getCurrentSession())));
    connect (mSessionBtn, &QPushButton::clicked, this, [=] () {switchWindow(SESSIONS_MENU);});
    if (mSessionsModel->rowCount () <= 1) {
        mSessionBtn->hide();
    }
#if DEBUG
    mSessionBtn->setStyleSheet(QString::fromUtf8 ("border:1px solid blue"));
#endif

    // 用户登录区
    mSwitchUserBtn = new QPushButton (this);
    mSwitchUserBtn->setObjectName (QStringLiteral("btnSwitchUser"));
    mSwitchUserBtn->setIcon (QIcon(":/images/images/switchUser.png"));
    mSwitchUserBtn->setIconSize (QSize(24, 24));
    mSwitchUserBtn->setFocusPolicy (Qt::NoFocus);
    mSwitchUserBtn->setFixedSize (GREETER_WINDOW_ICON_SIZE, GREETER_WINDOW_ICON_SIZE);
    mSwitchUserBtn->setCursor (Qt::PointingHandCursor);
    mSwitchUserBtn->installEventFilter (this);
    connect (mSwitchUserBtn, &QPushButton::clicked, this, [=] () {switchWindow(USERS_MENU);});
    if (mUsersModel->rowCount () < 2) {
        mSwitchUserBtn->hide();
    }

#if DEBUG
    mSwitchUserBtn->setStyleSheet(QString::fromUtf8 ("border:1px solid blue"));
#endif

    mLoginWnd = new LoginWindow (this);

#if DEBUG
    mLoginWnd->setStyleSheet(QString::fromUtf8 ("border:1px solid blue"));
#endif

#if DEBUG
    qDebug() << "User count:" << mUsersModel->rowCount();
    qDebug() << "=========== user's can login =============";
    for (int i = 0; i < mUsersModel->rowCount(); ++i) {
        qDebug() << "User ID:" << mUsersModel->data (mUsersModel->index (i, 0), UsersModel::UidRole).toString()
                << "User Name:" << mUsersModel->data (mUsersModel->index (i, 0), UsersModel::NameRole).toString();
    }
    qDebug() << "==========================================";
#endif

    setWindowOpacity(0.8);

    QString user = mUsersModel->getDefaultUser();
    QString session = mSessionsModel->getCurrentSession();
    QString face = mUsersModel->getUserFaceByName (user);

#if DEBUG
    qDebug() << "============= default info ===============";
    qDebug() << "user: " << user;
    qDebug() << "session: " << session;
    qDebug() << "face: " << face;
    qDebug() << "==========================================";

#endif

    // FIXME:// 验证相关, 使用 greeter
    mAuth = new GreeterAuth;

    // 使用默认用户、session
    mAuth->setUser (user);
    mAuth->setSession (session);
    mLoginWnd->setUserName (user);
    mLoginWnd->setFace (face);
    mSessionsModel->setCurrentSession (mAuth->getDefaultSession());

    // start authentication use greeter
    connect (mLoginWnd, &LoginWindow::startAuth, this, [=] (const QString& text) {
        mAuth->setPasswd (text);
        mAuth->authentication();
    });

    //connect (mAuth, &GreeterAuth::authSuccess, mLoginWnd, &LoginWindow::authSuccess);
    connect (mAuth, &GreeterAuth::authSuccess, this, [=] () {
        Q_EMIT mLoginWnd->authSuccess();
        mAuth->startSession();
    });

    connect (mAuth, &GreeterAuth::authFailed, this, [=] () {
        Q_EMIT mLoginWnd->authFailed (tr("Authentication failed. Please check whether the user name or password is correct."));
    });

    connect (mAuth, &GreeterAuth::requestPassword, this, [=] () {
        Q_EMIT mLoginWnd->authFailed (tr("Authentication failed. Please enter a user password."));
    });

    connect (mAuth, &GreeterAuth::requestUserName, this, [=] () {
        Q_EMIT mLoginWnd->authFailed (tr("Authentication failed. Please enter a user name."));
    });

    connect (mAuth, &GreeterAuth::showMessage, this, [=] (const QString& str) {
        Q_EMIT mLoginWnd->authFailed (str);
    });

    connect (mAuth, &GreeterAuth::cancelAuthentication, this, [=] () {
        Q_EMIT mLoginWnd->authFailed (tr("Cancel the validation."));
    });

    installEventFilter (this);
}

void GreeterWindow::setRootWindow ()
{
//    mGreeter->setRootWindow();
}

void GreeterWindow::resizeEvent (QResizeEvent *event)
{
    // 电源按钮位置,根据x，y的值进行计算，初始值为到右下角的距离
    int x = 39;
    int y = 66;

    QSize size = event->size();
    gScale = QString::number (size.width() / 1920.0, 'f', 1).toFloat();

    if (gScale > 1) {
        gScale = 1;
    }

    gFontSize = gScale > 0.5 ? 10 : 8;

    qDebug() << "graceful greeter resize to: " << size;

    int w = width ();
    int h = height ();

    mWidgetTime->move((w - mWidgetTime->geometry().width()) / 2, (int) (59 * gScale));

    if (mLoginWnd) {
        mLoginWnd->setChildrenGeometry ();
        mLoginWnd->setGeometry ((w - mLoginWnd->geometry().width()) / 2, h / 3, mLoginWnd->width(), h * 2 / 3);
    }

    // 电源按钮
    if (mPowerBtn) {
        x += mPowerBtn->width ();
        mPowerBtn->move (w - x,h - y);
        if (mPowerWnd) {
            mPowerWnd->move(mPowerBtn->x() - mPowerWnd->width() + mPowerBtn->width(),mPowerBtn->y() - mPowerWnd->height() - 3);
        }
    }

    // session 选择
    if (mSessionBtn) {
        x += (mSessionBtn->width() + 10);
        mSessionBtn->move(w - x, h - y);
        if (mSessionWnd) {
            mSessionWnd->move(mSessionBtn->x() - mSessionWnd->width() + mSessionBtn->width(),mSessionBtn->y() - mSessionWnd->height() - 3);
        }
    }

    // 用户切换按钮
    if (mSwitchUserBtn) {
        x += (mSwitchUserBtn->width() + 10);
        mSwitchUserBtn->move(w - x, h - y);
        if (mUserWnd) {
            mUserWnd->move (mSwitchUserBtn->x() - mUserWnd->width() + mSwitchUserBtn->width(), mSwitchUserBtn->y() - mUserWnd->height() - 3);
        }
    }
}

void GreeterWindow::showPowerWnd ()
{
    if (mPowerWnd && !mPowerWnd->isHidden()) {
        mPowerWnd->close();
        update();
        return;
    }

    mLoginWnd->hide ();

    mPowerWnd = new PowerManager (this);
    mPowerWnd->adjustSize ();

    mPowerWnd->setFixedSize (mPowerWnd->windowSize());
    mPowerWnd->move ((width()-mPowerWnd->width()) / 2, (height() - mPowerWnd->height()) / 2);

    connect(mPowerWnd, &PowerManager::switchToUser, this, [this]() {
        if (mPowerWnd && mPowerWnd->isVisible()) {
            mPowerWnd->close();
        }
    });

    mPowerWnd->setObjectName (QStringLiteral("powerWnd"));
    mPowerWnd->show ();
    update ();
}

void GreeterWindow::keyReleaseEvent (QKeyEvent* e)
{
    switch (e->key()) {
        case Qt::Key_K:
            if (e->modifiers() & Qt::ControlModifier) {
                if (mKeyboardBtn) mKeyboardBtn->click();
            }
            break;
        case Qt::Key_P:
            if (e->modifiers() & Qt::ControlModifier)
                if (mPowerBtn) mPowerBtn->click();
            break;
        case Qt::Key_Escape:
            switchWindow (NONE_MENU);
            break;
    }

    QWidget::keyReleaseEvent(e);
}

bool GreeterWindow::eventFilter (QObject *obj, QEvent *event)
{
    if ((event->type() == QEvent::MouseButtonPress) && (obj == this || obj == mLoginWnd)) {
        switchWindow (NONE_MENU);
    }

    return false;
}

void GreeterWindow::timedAutologin ()
{
//    if (mGreeter->isAuthenticated()) {
//        if (!mGreeter->authenticationUser().isEmpty())
//            mGreeter->startSession();
//        else if (mGreeter->autologinGuestHint())
//            mGreeter->startSession();
//        else if (!mGreeter->autologinUserHint().isEmpty())
//            mGreeter->authenticate(mGreeter->autologinUserHint());
//    } else {
//        mGreeter->authenticateAutologin();
//    }
}

void GreeterWindow::showSessionWnd ()
{
    if (!mSessionWnd) {
        mSessionWnd = new SessionWindow (mSessionsModel, this);
        QString session = mAuth->getSession();
        if (session.isNull() || session.isEmpty() || "" == session) {
            session = mUsersModel->getDefaultUser();
        }
        mSessionWnd->setCurrentSession (session);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
        mSessionWnd->setStyleSheet("QMenu::item{padding: 2px 10px 2px 10px;}");
#else
        mSessionWnd->setStyleSheet("QMenu::item{padding: 2px 10px 2px 30px;}");
#endif
        connect(mSessionWnd, &SessionWindow::sessionChanged, this, &GreeterWindow::onSessionChanged);
    }

    if (mSessionWnd->isVisible()) {
        mSessionWnd->close();
        mSessionWnd = nullptr;
        setFocus();
    } else {
        mSessionWnd->move(mSessionBtn->x() - mSessionWnd->width() + mSessionBtn->width(),mSessionBtn->y() - mSessionWnd->height() - 3);
        mSessionWnd->show();
        mSessionWnd->setFocus();
    }
    update();
}

void GreeterWindow::onAuthenticationComplete ()
{
//    if (mGreeter->isAuthenticated()) {
//        mGreeter->startSessionSync(mSessionsModel->getCurrentSession());
//    } else {
//        mGreeter->authenticate ("dingjing");
//        mLoginWnd->setUserName ("dingjing");
//        mLoginWnd->setFace (mUsersModel->getUserFaceByName ("dingjing"));
//        mLoginWnd->setEnabled (true);
//    }
}

void GreeterWindow::onUserSelected (const QModelIndex& index)
{
    //mLoginWnd->setUserIndex(index);

    switchWindow (USERS_MENU);
}

void GreeterWindow::onLanguageChanged (const QString& languageCode)
{
    return;
    if (languageCode == "")
        return ;

//    if (mGreeter->lang() == languageCode) {
//        return;
//    }

    qApp->removeTranslator (mConfiguration->mTrans);
    delete mConfiguration->mTrans;
    mConfiguration->mTrans = new QTranslator();
    QString qmFile;

    if (languageCode.startsWith("zh")) {
        qmFile = QM_DIR + QString("%1.qm").arg("zh_CN");
        setenv("LANGUAGE","zh_CN",1);
        setlocale(LC_ALL,"zh_CN.utf8");
    } else {
        qmFile = QM_DIR + QString("%1.qm").arg(languageCode);
        setenv("LANGUAGE",languageCode.toLatin1().data(),1);
        setlocale(LC_ALL,"");
    }
    mConfiguration->mTrans->load(qmFile);
    qApp->installTranslator(mConfiguration->mTrans);

//    mGreeter->setLang(languageCode);
}

void GreeterWindow::onSessionChanged (const QString &session)
{
    QString sessionTmp(session);
    if (!sessionIsValid(session)) {
        sessionTmp = mSessionsModel->getCurrentSession();
    }

    if (!sessionIsValid (sessionTmp)) {
        return;
    }

    QString sessionPrefix = sessionTmp.left(sessionTmp.indexOf('-'));
    if (sessionTmp == "") {
        sessionPrefix = "";
    }
    QString sessionIcon = QString(":/badges/badges/%1_badge.svg").arg(sessionPrefix.toLower());
    QFile iconFile(sessionIcon);
    if (!iconFile.exists()) {
        sessionIcon = QString(":/badges/badges/unknown_badge.svg");
    }

    if (mSessionBtn)    mSessionBtn->setIcon(QIcon(sessionIcon));


    mAuth->setSession (sessionTmp);
    mSessionWnd->setCurrentSession (sessionTmp);
    mSessionsModel->setCurrentSession (sessionTmp);

    if (mUserWnd) {
        mUserWnd->close();
        mUserWnd = nullptr;
    }

    if (mSessionWnd) {
        mSessionWnd->close();
        mSessionWnd = nullptr;
    }
}

void GreeterWindow::onUserChanged (const QString& user)
{
    if (user.isNull() || user.isEmpty()) {
        return;
    }

#if DEBUG
    qDebug() << "change to user:" << user
             << " face:" << mUsersModel->getUserFaceByName (user);
#endif

    mAuth->setUser (user);
    mLoginWnd->setUserName (user);
    mUserWnd->setCurrentUser (user);
    mUsersModel->setCurrentUser (user);
    mLoginWnd->setFace (mUsersModel->getUserFaceByName (user));

    switchWindow (USERS_MENU);
}

void GreeterWindow::onCurrentUserChanged (const QModelIndex& index)
{
//    for (int i = 0; i < mUserWnd->mUserList.count(); i++) {
//        auto entry = mUserWnd->mUserList.at(i);
//        if (entry->userIndex().data(QLightDM::UsersModel::NameRole).toString() == "*login") {
//            entry->setUserName(tr("Login"));
//        }
//    }
//
//    setBackground(index);
//
//    if (mGreeter->getDrawBackgroundIsStarted()) {
//        if (mFuture.isStarted()&& mFuture.isRunning()) {
//            mFuture.waitForFinished();
//        } else {
//            mFuture = QtConcurrent::run([=] () {
//                mGreeter->setRootWindow();
//            });
//        }
//    } else {
//        mFuture = QtConcurrent::run([=] () {
//            mGreeter->setRootWindow();
//        });
//    }
//
//    QString realName = index.data(QLightDM::UsersModel::NameRole).toString();
//    bool isLogin = index.data(QLightDM::UsersModel::LoggedInRole).toBool();
//
//    if (isLogin || realName == "*login" || realName == "*guest") {
//        if (mSessionBtn) {
//            mSessionBtn->hide();
//        }
//    } else {
//        if (mSessionBtn) {
//            mSessionBtn->show();
//        }
//    }
//
//    if (realName == "*guest" || realName == "*login")
//        return;
//
//    updateLanguage(realName);
//    if (!mSessionHasChanged && mSessionsModel->rowCount() > 1) {
//        QString session = index.data(QLightDM::UsersModel::SessionRole).toString();
//        onSessionChanged(session);
//    }

    update();
}

void GreeterWindow::onUserChangedByManual (const QString &userName)
{
//    mUserWnd->setCurrentUser(userName, true);

    updateLanguage(userName);
    updateSession(userName);
}

void GreeterWindow::updateSession (QString userName)
{
    QDBusInterface iface("org.freedesktop.Accounts", "/org/freedesktop/Accounts",
                         "org.freedesktop.Accounts",QDBusConnection::systemBus());
    QDBusReply<QDBusObjectPath> userPath = iface.call("FindUserByName", userName);
    if (!userPath.isValid()) {
        qWarning () << "Get UserPath error:" << userPath.error ();
    } else {
        QDBusInterface userIface("org.freedesktop.Accounts", userPath.value().path(),
                                 "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
        QDBusReply<QDBusVariant> sessionReply = userIface.call("Get", "org.freedesktop.Accounts.User", "XSession");
        if (!sessionReply.isValid()) {
            qWarning () << "Get User's session error" << sessionReply.error ();
        } else {
            QString session = sessionReply.value().variant().toString();
            onSessionChanged(session);
        }
    }
}

void GreeterWindow::updateLanguage (QString userName)
{
    QString language;
    QString formatsLocale;
    QDBusInterface iface ("org.freedesktop.Accounts", "/org/freedesktop/Accounts",
                          "org.freedesktop.Accounts", QDBusConnection::systemBus ());
    QDBusReply<QDBusObjectPath> userPath = iface.call ("FindUserByName", userName);
    if (!userPath.isValid ()) {
        qWarning () << "Get UserPath error:" << userPath.error ();
    } else {
        QDBusInterface userIface ("org.freedesktop.Accounts", userPath.value ().path (),
                                  "org.freedesktop.DBus.Properties", QDBusConnection::systemBus ());
        QDBusReply<QDBusVariant> languageReply = userIface.call ("Get", "org.freedesktop.Accounts.User", "Language");
        if (!languageReply.isValid ()) {
            qWarning () << "Get User's language error" << languageReply.error ();
        } else {
            language = languageReply.value().variant().toString();
            if (!language.isEmpty()) {
                onLanguageChanged(language);
            }
        }

        QDBusReply<QDBusVariant> fomatsLocalReply = userIface.call("Get", "org.freedesktop.Accounts.User", "FormatsLocale");
        if (!fomatsLocalReply.isValid()) {
            qWarning () << "Get User's language error" << languageReply.error ();
        } else {
            formatsLocale = fomatsLocalReply.value().variant().toString();
            if (!formatsLocale.isEmpty()) {
                if(formatsLocale.startsWith("zh")){
                    mLocal = QLocale::Chinese;
                    QString date = mLocal.toString(QDate::currentDate(),"yyyy/MM/dd ddd");
                    mLBLDate->setText(date);
                } else {
                    mLocal = QLocale::English;
                    QString date = mLocal.toString(QDate::currentDate(),"yyyy/MM/dd ddd");
                    mLBLDate->setText(date);
                }
            }
        }
    }
}

QString GreeterWindow::getAccountBackground (uid_t uid)
{
    QDBusInterface iface("org.freedesktop.Accounts", "/org/freedesktop/Accounts",
                         "org.freedesktop.Accounts",QDBusConnection::systemBus());

    QDBusReply<QDBusObjectPath> userPath = iface.call("FindUserById", (qint64)uid);
    if (!userPath.isValid()) {
        qWarning () << "Get UserPath error:" << userPath.error ();
    } else {
        QDBusInterface userIface ("org.freedesktop.Accounts", userPath.value().path(),
                                 "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
        QDBusReply<QDBusVariant> backgroundReply = userIface.call("Get", "org.freedesktop.Accounts.User", "BackgroundFile");
        if (backgroundReply.isValid()) {
            return backgroundReply.value().variant().toString();
        }
    }
    return "";
}

bool GreeterWindow::sessionIsValid (const QString &session)
{
    if (session.isNull() || session.isEmpty()) {
        return false;
    }

    int count = mSessionsModel->rowCount();
    for (int i = 0; i < count; i++) {
        QString sessionKey = mSessionsModel->index(i, 0).data(SessionsModel::KeyRole).toString();
        if(sessionKey == session)
            return true;
    }

    return false;
}

void GreeterWindow::showUserWnd ()
{
    if (!mUserWnd) {
        mUserWnd = new UserWindow (mUsersModel, this);
        QString user = mAuth->getUser();
        if (user.isNull() || user.isEmpty() || "" == user) {
            user = mUsersModel->getDefaultUser();
        }
        mUserWnd->setCurrentUser (user);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
        mUserWnd->setStyleSheet("QMenu::item{padding:2px 10px 2px 10px;}");
#else
        mUserWnd->setStyleSheet("QMenu::item{padding: 2px 10px 2px 30px;}");
#endif
        connect(mUserWnd, &UserWindow::userChanged, this, &GreeterWindow::onUserChanged);
    }

    if (mUserWnd->isVisible()) {
        mUserWnd->close();
        mUserWnd = nullptr;
        setFocus();
    } else {
        mUserWnd->move (mSwitchUserBtn->x() - mUserWnd->width() + mSessionBtn->width(), mSwitchUserBtn->y() - mUserWnd->height() - 3);
        mUserWnd->show();
        mUserWnd->setFocus();
    }
    update();
}

GreeterWindow::~GreeterWindow ()
{
    delete mUsersModel;
    mUsersModel = nullptr;

    delete mAuth;
    mAuth = nullptr;
}

void GreeterWindow::switchWindow (GreeterWindow::MenuType type)
{
    bool userFlag = false;
    bool sessionFlag = false;

    if (mUserWnd) {
        userFlag = mUserWnd->isVisible();
        mUserWnd->close();
        mUserWnd = nullptr;
    }

    if (mSessionWnd) {
        sessionFlag = mSessionWnd->isVisible();
        mSessionWnd->close();
        mSessionWnd = nullptr;
    }

    if (mPowerWnd) {
        mPowerWnd->close();
        mPowerWnd = nullptr;
    }

    switch (type) {
        case POWER_MENU:
            showPowerWnd();
            break;
        case SESSIONS_MENU:
            if (!sessionFlag) {
                showSessionWnd ();
            }
            mLoginWnd->show();
            break;
        case USERS_MENU:
            if (!userFlag) {
                showUserWnd();
            }
            mLoginWnd->show();
            break;
        default:
            break;
    }
}




