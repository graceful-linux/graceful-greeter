//
// Created by dingjing on 4/5/22.
//

#include "login-window.h"

#include <QDebug>
#include <QTimer>
#include <QLabel>
#include <QPainter>
#include <QKeyEvent>
#include <QGridLayout>
#include <QPushButton>
#include <QListWidget>
#include <QPainterPath>
#include <QAbstractItemModel>
#include <QtConcurrent/QtConcurrentRun>

#include "../ui/widget/icon-edit.h"

#include "../common/global.h"
#include "../common/configuration.h"

#include <pwd.h>
#include <fcntl.h>
#include <unistd.h>
#include <clocale>
#include <sys/types.h>
#include "../greeter/pam-tally.h"
#include "../model/users-model.h"

#define _(string) gettext(string)

/**
 * @brief
 *  登录框，包含用户名、密码输入、头像显示
 */
LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    mIsLockingFlg(false),
    mTimer(nullptr)
{
#if DEBUG
    setStyleSheet(QString::fromUtf8 ("border:1px solid red"));
#endif

    mUserWidget = new QWidget(this);
    mUserWidget->setObjectName(QStringLiteral("userWidget"));

    // 头像
    mFaceLabel = new QLabel(mUserWidget);
    const QString SheetStyle = QString("border-radius: %1px;  border:0px solid white;").arg(90 * gScale);
    mFaceLabel->setObjectName(QStringLiteral("faceLabel"));
    mFaceLabel->setFocusPolicy(Qt::NoFocus);
    mFaceLabel->setStyleSheet(SheetStyle);
    mFaceLabel->setAlignment(Qt::AlignCenter);

    // 用户名
    mNameLabel = new QLabel(mUserWidget);
    mNameLabel->setObjectName(QStringLiteral("login_nameLabel"));
    mNameLabel->setFocusPolicy(Qt::NoFocus);
    mNameLabel->setAlignment(Qt::AlignCenter);

    // 密码框所在窗口
    mPasswdWidget = new QWidget(this);
    mPasswordEdit = new IconEdit(mPasswdWidget);
    mPasswdWidget->setObjectName(QStringLiteral("passwordWidget"));
    mPasswordEdit->setObjectName(QStringLiteral("passwordEdit"));
    mPasswordEdit->setIcon(QIcon(":/images/images/login-button.png"));
    mPasswordEdit->setFocusPolicy(Qt::StrongFocus);
    mPasswordEdit->installEventFilter(this);
    setFocusProxy (mPasswordEdit);
    mPasswordEdit->setEnabled (true);
    mPasswordEdit->setVisible (true);

    // 消息框
    mMessageLabel = new QLabel(mPasswdWidget);
    mMessageButton = new QPushButton(mPasswdWidget);
    mMessageLabel->setObjectName(QStringLiteral("messageLabel"));
    mMessageLabel->setAlignment(Qt::AlignCenter);
    mMessageButton->setObjectName(QStringLiteral("messageButton"));
    mMessageButton->hide();

    connect (mPasswordEdit, &IconEdit::startAuthentication, this, [=] (const QString& str) {
        clearMessage();
        mPasswordEdit->startWaiting();

        Q_EMIT startAuth(str);
    });
    connect (this, &LoginWindow::authSuccess, this, [=] () {
        clearMessage();
        mPasswordEdit->clear();
        mPasswordEdit->stopWaiting();
    });
    connect (this, &LoginWindow::authFailed, this, [=] (const QString& str) {
        clearMessage();

        QString strDisplay = str;
        mMessageLabel->setToolTip(str);
        QFontMetrics font(mMessageLabel->font());
        int fontSize = font.horizontalAdvance (str);
        if (fontSize > 75) {                // 当字符宽度大于75，不再显示完整信息
            strDisplay = font.elidedText(str, Qt::ElideRight, 600);
        }
        mMessageLabel->setText(strDisplay);
        mPasswordEdit->clear();
        mPasswordEdit->stopWaiting();
    });

    pam_tally_init ();
}

QPixmap LoginWindow::PixmapToRound(const QPixmap &src, int radius)
{
    if (src.isNull()) {
        return QPixmap();
    }

    const QPixmap& pixmapA (src);
    QPixmap pixmap(radius * 2,radius * 2);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    path.addEllipse(0, 0, radius * 2, radius * 2);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, radius * 2, radius * 2, pixmapA);

    return pixmap;
}

void LoginWindow::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
}

void LoginWindow::resizeEvent(QResizeEvent *)
{
    setChildrenGeometry();
}

void LoginWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        showPasswordAuthWidget();
    }

    QWidget::keyReleaseEvent(event);
}

void LoginWindow::setChildrenGeometry()
{
    mUserWidget->setGeometry(0, 0, width(), (int)(292 * gScale));

    const QString SheetStyle = QString("border-radius: %1px;border:0px solid white;").arg(90 * gScale);
    mFaceLabel->setStyleSheet(SheetStyle);

    setFace(mFace);
    mFaceLabel->setGeometry((int) (((float)width() - 180 * gScale) / 2) , 0, (int) (180 * gScale), (int) (180 * gScale));
    mNameLabel->setGeometry(0, (int)((float)mFaceLabel->geometry().bottom() + 25 * gScale), width(), 40);

    if (gScale < 0.5) {
        setFixedWidth (360);
    } else {
        setFixedWidth (600);
    }

    mPasswdWidget->setGeometry (0, (int)((float)mNameLabel->geometry().bottom() + 45 * gScale), width(), 150);
    mPasswordEdit->setGeometry ((mPasswdWidget->width() - 300) / 2, 0, 300, 34);
    mMessageLabel->setGeometry ((mPasswdWidget->width() - 600) / 2, mPasswordEdit->geometry().bottom() + 25, 600, 25);
    mMessageButton->setGeometry ((mPasswdWidget->width() - 200) / 2, 0, 200, 40);
}

void LoginWindow::clearMessage()
{
    mMessageLabel->clear();
}

void LoginWindow::setUserName(const QString& userName)
{
    if (userName.isNull() || userName.isEmpty() || "" == userName) {
        qDebug() << "set user name is null";
        return;
    }

    // FIXME:// 此处获取全名
    mName = userName;

    mNameLabel->setText(userName);
}

void LoginWindow::setFace (const QString& facePath)
{
    QPixmap faceImage;
    mFaceLabel->setFixedSize ((int)(180 * gScale), (int)(180 * gScale));
    mFaceLabel->setPixmap (QPixmap());

    faceImage = scaledPixmap ((int)(180 * gScale), (int)(180 * gScale), facePath);
    faceImage =  PixmapToRound (faceImage, (int)(90 * gScale));

    mFaceLabel->setAlignment(Qt::AlignCenter);
    mFaceLabel->setPixmap (faceImage);

    mFace = facePath;
}

void LoginWindow::showPasswordAuthWidget()
{
#if DEBUG
    qDebug() << "show passwd edit line ...";
#endif
    mPasswdWidget->setEnabled (true);
    mPasswdWidget->setVisible (true);
}

void LoginWindow::unlock_countdown()
{
#if DEBUG
    qDebug() << "";
#endif

#if 0
    int failed_count = 0;
    int time_left = 0;
    int deny = 0;
    int fail_time =0;
    int unlock_time = 0;

    if (nullptr == mName.toLatin1().data()) {
        qDebug() << "获取用户信息失败!";
        return;
    }

    struct passwd* user = nullptr;
    user = getpwnam(mName.toLatin1().data());

    pam_tally_unlock_time_left(user->pw_uid, &failed_count, &time_left, &deny,&fail_time,&unlock_time);

    if (time_left / 60 > 0) {
        char ch[100] = {0};
        int nMinute = time_left/60 + 1;
        mMessageLabel->setText(tr("Please try again in %1 minutes.").arg(nMinute));
        mMessageLabel->setToolTip(tr("Please try again in %1 minutes.").arg(nMinute));
//        mPasswordEdit->clearText();
//        mPasswordEdit->setDisabled(true);
        mIsLockingFlg = true;
        return ;
    } else if (time_left > 0) {
        char ch[100] = {0};
        mMessageLabel->setText(tr("Please try again in %1 seconds.").arg(time_left%60));
        mMessageLabel->setToolTip(tr("Please try again in %1 seconds.").arg(time_left%60));
//        mPasswordEdit->clearText();
//        mPasswordEdit->setDisabled(true);
        mIsLockingFlg = true;
        return ;
    } else if (failed_count == 0xFFFF) {
        mMessageLabel->setText(tr("Account locked permanently."));
        mMessageLabel->setToolTip(tr("Account locked permanently."));
//        mPasswordEdit->clearText();
//        mPasswordEdit->setDisabled(true);
        mIsLockingFlg = true;
        return ;
    } else {
        if (mPasswordEdit) {
//            mPasswordEdit->setDisabled(false);
            mPasswordEdit->setFocus();
        }

        if (mIsLockingFlg) {
            onShowMessage(tr("Authentication failure, Please try again"), Greeter::MessageTypeInfo);
            mIsLockingFlg = false;
        }

        mTimer->stop();
    }
#endif
}

LoginWindow::~LoginWindow ()
{

}
