//
// Created by dingjing on 4/16/22.
//

#ifndef GRACEFUL_GREETER_GREETER_AUTH_H
#define GRACEFUL_GREETER_GREETER_AUTH_H
#include "authentication-interface.h"

class GreeterAuthPrivate;

/**
 * @brief
 *  greeter 验证过程如下：
 *    1. 输入用户名
 *    2. 输入密码
 *    3. 输入要拉起的session
 *    4. 开始 '验证密码'
 *      1. 如果成功：拉起 session
 *      2. 如果失败：将密码值为 '空'，给用户提示，要求 '检查用户名' 或 '重新输入密码'
 */
class GreeterAuth : public AuthenticationInterface
{
    Q_OBJECT
public:
    explicit GreeterAuth (AuthenticationInterface* parent = nullptr);
    ~GreeterAuth() override;

    bool isAuthenticated () override;
    void setUser (const QString& user) override;
    void setPasswd (const QString& passwd) override;
    void setSession (const QString& session) override;

    QString getDefaultSession () override;

    bool startSession () override;
    void authentication () override;

protected:
    QString getAuthType () override;

private:
    GreeterAuthPrivate                 *d_ptr;
    Q_DECLARE_PRIVATE(GreeterAuth);
};


#endif //GRACEFUL_GREETER_GREETER_AUTH_H
