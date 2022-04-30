//
// Created by dingjing on 4/5/22.
//

#ifndef GRACEFUL_GREETER_LOGIN_WINDOW_H
#define GRACEFUL_GREETER_LOGIN_WINDOW_H
#include <QMap>
#include <QWidget>

#include <QMap>
#include <QWidget>

#include "../greeter/pam-tally.h"

class QTimer;
class QLabel;
class QPushButton;
class QListWidget;

class IconEdit;

class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow() override;

    /**
     * @brief
     *  设置用户头像
     */
    void setFace(const QString& faceFile);

    /**
     * @brief
     *  设置用户名
     */
    void setUserName(const QString& userName);

    void setChildrenGeometry();

protected:
    void showEvent(QShowEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void keyReleaseEvent(QKeyEvent *event) override;

Q_SIGNALS:
    void authSuccess ();
    void authFailed (const QString&);
    void startAuth (const QString& text);

private:
    void clearMessage();
    void unlock_countdown();
    void showPasswordAuthWidget();
    static QPixmap PixmapToRound(const QPixmap &src, int radius);

private:
    QString                                 mName;                                  // mNameLabel show 'real name', this value save 'name'
    qint32                                  mUid{};                                 // 用户id

    enum AuthMode { PASSWORD, UNKNOWN };

    AuthMode authMode = UNKNOWN;

    // UI
    QWidget                                *mUserWidget = nullptr;              // 放置用户信息Label
    QLabel                                 *mFaceLabel = nullptr;               // 头像
    QLabel                                 *mNameLabel = nullptr;               // 用户名
    QLabel                                 *mIsLoginLabel = nullptr;            // 提示是否已登录

    QWidget                                *mPasswdWidget;                      // 放置密码输入框和信息列表
    IconEdit                               *mPasswordEdit;                      // 密码输入框
    QLabel                                 *mMessageLabel;                      // PAM消息显示
    QPushButton                            *mMessageButton;

    QString                                 mFace;

    bool                                    mIsLockingFlg;                      // 判断当前是否正在锁定倒计时
    QTimer                                 *mTimer;
};

#endif //GRACEFUL_GREETER_LOGIN_WINDOW_H
