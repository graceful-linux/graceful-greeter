//
// Created by dingjing on 4/17/22.
//

#include "../app/ui/widget/icon-edit.h"

#include <QFile>
#include <QApplication>

int main (int argc, char* argv[])
{
    QApplication app (argc, argv);

    QFile qssFile (":/styles/styles/graceful-greeter.qss");
    qssFile.open (QFile::ReadOnly);
    QByteArray qssByte = qssFile.readAll();
    qssFile.close();
    app.setStyleSheet (qssByte);


    auto ie = new IconEdit;

    ie->show();

    return QApplication::exec();
}