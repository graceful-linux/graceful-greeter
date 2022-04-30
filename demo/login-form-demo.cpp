//
// Created by dingjing on 2022/3/3.
//

#include <QApplication>
#include "login-form.h"

using namespace graceful;

int main (int argc, char* argv[])
{
    QApplication app (argc, argv);

    LoginForm lf;
    lf.show();

    return app.exec();
}