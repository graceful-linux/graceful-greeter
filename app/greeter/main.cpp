//
// Created by dingjing on 4/2/22.
//

#include <QApplication>

#include <QFile>
#include <QtGlobal>
#include <QDateTime>
#include <QResource>
#include <QTextCodec>

#include "../ui/main-window.h"

#include "../common/log.h"
#include "../common/global.h"
#include "../common/configuration.h"

#include <glib.h>
#include <glib/gprintf.h>

#if DEBUG
#include <syslog.h>
#endif 

int             gFontSize = 8;
float           gScale = 1.0;

static void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main (int argc, char* argv[])
{
    QApplication app (argc, argv);

    qInstallMessageHandler(outputMessage);
    QDateTime startDateTime = QDateTime::currentDateTime();
    QByteArray time = QString ("[%1] ").arg(startDateTime.toString("MM-dd hh:mm:ss.zzz")).toLocal8Bit();

    const static char* logDir = "/var/log/";
    if (!QFile::exists(logDir)) {
        g_mkdir_with_parents (logDir, 0777);
    }

#if !DEBUG
    log_init (LOG_TYPE_FILE, LOG_DEBUG, LOG_ROTATE_FALSE, -1, logDir, APP_NAME, "log");
#endif

    qInfo() << "=================> 开始执行 - " APP_NAME " <=================";

#if (QT_VERSION>=QT_VERSION_CHECK (5,6,0))
    QCoreApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute (Qt::AA_UseHighDpiPixmaps);
#endif

    QTextCodec *codec = QTextCodec::codecForName ("UTF-8");
    QTextCodec::setCodecForLocale (codec);

    Configuration *configuration = Configuration::instance();
    configuration->mTrans = new QTranslator();
    configuration->mTrans->load (QM_DIR + QString("%1.qm").arg(QLocale::system().name()));

    QApplication::instance()->installTranslator (configuration->mTrans);

    QFile qssFile (":/styles/styles/graceful-greeter.qss");
    qssFile.open (QFile::ReadOnly);
    QByteArray qssByte = qssFile.readAll();
    qssFile.close();
    app.setStyleSheet (qssByte);

    MainWindow win;

    win.show();
    win.activateWindow();

    return QApplication::exec();
}

static void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)

    QByteArray localMsg = msg.toLocal8Bit();

#if DEBUG
    syslog (LOG_ERR, "[%s] [DEBUG] [%s:%d] %s %s", APP_NAME, context.file, context.line, context.function, localMsg.constData());
    return;
#else
    switch(type) {
        case QtDebugMsg:
            log_print(LOG_DEBUG, APP_NAME, context.file, context.line, context.function, "%s", localMsg.constData());
            break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        case QtInfoMsg:
            log_print(LOG_INFO, APP_NAME, context.file, context.line, context.function, "%s", localMsg.constData());
            break;
#endif
        case QtWarningMsg:
            log_print(LOG_WARNING, APP_NAME, context.file, context.line, context.function, "%s", localMsg.constData());
            break;
        case QtCriticalMsg:
            log_print(LOG_ERR, APP_NAME, context.file, context.line, context.function, "%s", localMsg.constData());
            break;
        case QtFatalMsg:
            log_print(LOG_CRIT, APP_NAME, context.file, context.line, context.function, "%s", localMsg.constData());
            abort();
    }
#endif
}
