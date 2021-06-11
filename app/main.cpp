#include <QIcon>
#include <QtDebug>
#include <QtGlobal>
#include <iostream>
#include <QSettings>
#include <QDesktopWidget>
#include <QtWidgets/QApplication>

#include "settings.h"
#include "main-window.h"

QFile logfile;
QTextStream ts;


using namespace graceful;

void messageHandler(QtMsgType type, const QMessageLogContext&, const QString& msg)
{
    std::cerr << type << ": " << msg.toLatin1().data() << "\n";
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageHandler);

    Cache::prepare();

    QApplication a(argc, argv);

    if (!Settings().iconThemeName().isEmpty()) {
        QIcon::setThemeName(Settings().iconThemeName());
    }

    MainWindow* focusWindow = nullptr;
    for (int i = 0; i < QApplication::desktop()->screenCount(); ++i) {
        MainWindow *w = new MainWindow(i);
        w->show();
        if (w->showLoginForm()) {
            focusWindow = w;
        }
    }

    if (focusWindow) {
        focusWindow->setFocus(Qt::OtherFocusReason);
        focusWindow->activateWindow();
    }

    return a.exec();
}
