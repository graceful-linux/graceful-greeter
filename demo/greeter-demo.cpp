//
// Created by dingjing on 4/16/22.
//

#include "../app/authentication/greeter-auth.h"

#include <unistd.h>

#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QApplication>

/**
 * @brief 
 *
 */

int main (int argc, char* argv[])
{
    QApplication app (argc, argv);

    AuthenticationInterface* auth = new GreeterAuth;

    qDebug() << auth->getAuthType();

    auth->setUser ("aa");
    auth->setPasswd("kk");
    auth->setSession("gnome");
    auth->authentication();

    auth->connect (auth, &AuthenticationInterface::authSuccess, [=] () {
        qDebug() << "auth successful!!!";
        auth->startSession();
    });

    auth->connect (auth, &AuthenticationInterface::authFailed, [=] () {
        qDebug() << "auth failed!!!";
        auth->setPasswd("qwer1234");
        auth->authentication();
    });


    return QApplication::exec();
}
