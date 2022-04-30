//
// Created by dingjing on 4/4/22.
//

#include "main-window.h"

#include <QDir>
#include <QtMath>
#include <QTimer>
#include <QScreen>
#include <QWindow>
#include <QPainter>
#include <QX11Info>
#include <QProcess>
#include <QKeyEvent>
#include <QByteArray>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>

#include <unistd.h>
#include <xcb/xcb.h>
#include <libintl.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xrandr.h>

#include "../common/global.h"
#include "../common/configuration.h"
#include "../common/xevent-monitor.h"
#include "../common/monitor-watcher.h"
#include "../display-switch/display-service.h"

#include "../ui/greeter-window.h"

static QPixmap* blurPixmap (QPixmap *pixmap);

bool MainWindow::mFirst = true;

QT_BEGIN_NAMESPACE
extern void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

#define BLUR_RADIUS 300

MainWindow::MainWindow (QWidget *parent)
    : QWidget(parent),
    mScreenModel(new ScreenModel(this)),
    mConfiguration(Configuration::instance()),
    mActiveScreen(nullptr),
    mBackground(nullptr),
    mMonitorCount(0)
{
    setWindowFlags (Qt::X11BypassWindowManagerHint);
    XRRQueryExtension (QX11Info::display(), &mRREventBase, &mRRErrorBase);
    XRRSelectInput (QX11Info::display(), QX11Info::appRootWindow(), RRScreenChangeNotifyMask);

    QDesktopWidget* desktop = QApplication::desktop();
    connect (desktop, &QDesktopWidget::resized, this, &MainWindow::onScreenResized);
    connect (desktop, &QDesktopWidget::screenCountChanged, this, &MainWindow::onScreenResized);

    setGeometry (desktop->geometry());

    setMouseTracking (true);

    mDefaultBackgroundPath = Configuration::getDefaultBackgroundName();
    bool drawUserBackground = mConfiguration->getValue("draw-user-background").toBool();
    if (drawUserBackground) {
        mBackgroundMode = DRAW_USER_BACKGROUND;
    } else {
        mBackground = QSharedPointer<Background>(new Background);
        QString backgroundPath = mConfiguration->getValue("background").toString();
        if (!backgroundPath.isEmpty()) {
            mBackgroundMode = DRAW_BACKGROUND;
            mBackground->type = BACKGROUND_IMAGE;
            mBackground->image = backgroundPath;
        } else {
            QString color = mConfiguration->getValue("background-color").toString();
            if (!color.isEmpty()) {
                mBackgroundMode = DRAW_COLOR;
                mBackground->type = BACKGROUND_COLOR;
                mBackground->color = color;
            } else {
                mBackgroundMode = DRAW_DEFAULT;
                mBackground->type = BACKGROUND_IMAGE;
                mBackground->image = mDefaultBackgroundPath;
            }
        }
    }

    mTransition.started = false;

    // 登录界面
    mGreeterWnd = new GreeterWindow (this);
    QString backgroundName = "";
    QString resolution = QString ("%1x%2").arg(QApplication::primaryScreen()->geometry().width()).arg(QApplication::primaryScreen()->geometry().height());
    QPair<QString, QString> key (backgroundName, resolution);
    if (!mBackgrounds.contains(key)) {
        auto pixmap =  new QPixmap (scaledPixmap(QApplication::primaryScreen()->geometry().width(), QApplication::primaryScreen()->geometry().height(), backgroundName));
        mBackgrounds[key] = blurPixmap (pixmap);
    }

    // 登录界面移动到鼠标所在的屏幕上
    if (QApplication::screens().count() > 1) {
        QPoint point = QCursor::pos();
        QScreen *curScreen = nullptr;
        for (QScreen *screen : QApplication::screens()) {
            QRect screenRect = screen->geometry();
            if (screenRect.contains(point)) {
                curScreen = screen;
                break;
            }
        }

        if (curScreen != mActiveScreen && curScreen != nullptr) {
            moveToScreen(curScreen);
        }
    } else {
        moveToScreen(QApplication::primaryScreen());
    }

    QApplication::instance()->installNativeEventFilter (this);

    showLater();
}

void MainWindow::setBackground (QSharedPointer<Background>& background)
{
    if (mBackgroundMode != DRAW_USER_BACKGROUND) {
        return;
    }

    stopTransition();

    if (background) {
        if (background->image.isEmpty()) {
            background->image = mDefaultBackgroundPath;
        }
    }

    if (mBackground && background && mBackground->image == background->image) {
        repaint();
        return;
    }

    if (!mBackground.isNull()) {
        startTransition (mBackground, background);
    }

    mBackground = background;
}

void MainWindow::paintEvent (QPaintEvent* e)
{
    for (auto screen : QApplication::screens()) {
        QRect rect = screen->geometry();

        if (mTransition.started) {
            drawTransitionAlpha (rect);
        } else {
            drawBackground (mBackground, rect);
        }

        QPainter painter(this);
    }

    return QWidget::paintEvent(e);
}

void MainWindow::mouseMoveEvent (QMouseEvent* e)
{
    if (QApplication::screens().count() > 1) {
        QPoint point = e->pos();
        QScreen *curScreen = nullptr;
        for (QScreen *screen : QApplication::screens()) {
            QRect screenRect = screen->geometry();
            if (screenRect.contains(point)) {
                curScreen = screen;
                break;
            }
        }

        if (curScreen != mActiveScreen && curScreen != nullptr) {
            moveToScreen(curScreen);
        }
    }

    return QWidget::mouseMoveEvent (e);
}

bool MainWindow::nativeEventFilter (const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);
    if (qstrcmp(eventType, "xcb_generic_event_t") != 0) {
        return false;
    }
    auto event = reinterpret_cast<xcb_generic_event_t*>(message);
    const uint8_t responseType = event->response_type & ~0x80;
    if (responseType == mRREventBase + RRScreenChangeNotify) {
        RRScreenChangeEvent();
    }

    return false;
}

void MainWindow::onTransition ()
{
    mTransition.stage += 0.1;

    if (mTransition.stage >= 1.0) {
        stopTransition();
    }

    update();
}

void MainWindow::onScreenResized ()
{
    hide();
    QDesktopWidget *desktop = QApplication::desktop();
    setGeometry(desktop->geometry());

    moveToScreen(QApplication::primaryScreen());
    show();
    mGreeterWnd->setRootWindow();
}

void MainWindow::screenCountEvent ()
{
    QDesktopWidget *desktop = QApplication::desktop();
    setGeometry(desktop->geometry());

    moveToScreen(QApplication::primaryScreen());

    repaint();
}

void MainWindow::RRScreenChangeEvent ()
{
    XRRScreenResources* screen = nullptr;
    screen = XRRGetScreenResources (QX11Info::display(), QX11Info::appRootWindow());
    XRROutputInfo *info;
    int count = 0;

    for (int i = 0; i < screen->noutput; i++) {
        info = XRRGetOutputInfo(QX11Info::display(), screen, screen->outputs[i]);
        if (info->connection == RR_Connected) {
            count++;
        }
        XRRFreeOutputInfo(info);
    }

    onScreenCountChanged(count);
    XRRFreeScreenResources(screen);
}

void MainWindow::onScreenCountChanged (int newCount)
{
    if (newCount == mMonitorCount) {
        return;
    }

    mMonitorCount = newCount;

    if (newCount < 2) {
        QProcess enableMonitors;
        //默认设置显示最大分辨率
        enableMonitors.start ("xrandr --auto");
        enableMonitors.waitForFinished (-1);
        QTimer::singleShot (600, [=] () {
            QProcess enableMonitors;
            enableMonitors.start (QString("xrandr --output %1 --auto").arg(QApplication::primaryScreen()->name()));
            enableMonitors.waitForFinished (-1);
        });
    } else {
        DisplayService displayService;
        int mode = mConfiguration->getValue("display-mode").toInt();
        displayService.switchDisplayMode ((DisplayMode)mode);
    }

    // 在调用xrandr打开显示器以后，不能马上设置窗口大小，会设置不正确的
    // 分辨率，延时500ms正常。
    QTimer::singleShot(500, this, SLOT(screenCountEvent()));
}

void MainWindow::showLater ()
{
    if (checkNumLockState()) {
        unsigned int numMask = XkbKeysymToModifiers (QX11Info::display(), XK_Num_Lock);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, numMask, 0);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, numMask, numMask);
    } else {
        unsigned int numMask = XkbKeysymToModifiers (QX11Info::display(), XK_Num_Lock);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, numMask, numMask);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, numMask, 0);
    }

    if (checkCapsState()) {
        unsigned int capsMask = XkbKeysymToModifiers (QX11Info::display(), XK_Caps_Lock);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, capsMask, 0);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, capsMask, capsMask);
    } else {
        unsigned int capsMask = XkbKeysymToModifiers (QX11Info::display(), XK_Caps_Lock);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, capsMask, capsMask);
        XkbLockModifiers (QX11Info::display(), XkbUseCoreKbd, capsMask, 0);
    }

    XDefineCursor (QX11Info::display(), QX11Info::appRootWindow(), XCreateFontCursor(QX11Info::display(), XC_arrow));

    bindtextdomain("Linux-PAM","/usr/share/locale");
    textdomain("Linux-PAM");

    XEventMonitor::instance()->start();
}

void MainWindow::stopTransition ()
{
    if (mTimer && mTimer->isActive()) {
        mTimer->stop();
    }

    mTransition.stage = 1.0;
    mTransition.started = false;
}

void MainWindow::drawTransitionAlpha (const QRect &rect)
{
    drawBackground(mTransition.from, rect, (float)1.0 - mTransition.stage);
    drawBackground(mTransition.to, rect, mTransition.stage);
}

void MainWindow::moveToScreen (QScreen *screen)
{
    mActiveScreen = screen;
    QRect activeScreenRect = mActiveScreen->geometry();

    if (1 == QApplication::screens().count()) {
        mGreeterWnd->setGeometry(QApplication::primaryScreen()->geometry());
    } else {
        mGreeterWnd->setGeometry(activeScreenRect);
    }

    update();
}

QPixmap *MainWindow::getBackground (const QString &path, const QRect &rect)
{
    QString resolution = QString ("%1x%2").arg(rect.width()).arg(rect.height());
    QPair<QString, QString> key (path, resolution);

    if (mBackgrounds.isEmpty () && mFuture.isRunning ()) {
        mFuture.waitForFinished();
    } else {
        if (!mFuture.isFinished () && mFuture.isStarted ()) {
            mFuture.waitForFinished ();
        }
    }

    if (!mBackgrounds.contains (key)) {
        auto pixmap = new QPixmap (scaledPixmap(width (), height (), path));
        mBackgrounds[key] = blurPixmap (pixmap);
    }

    return mBackgrounds [key];
}

void MainWindow::startTransition (QSharedPointer<Background>& from, QSharedPointer<Background>& to)
{
    if (!mTimer) {
        mTimer = new QTimer (this);
        connect (mTimer, &QTimer::timeout, this, &MainWindow::onTransition);
    }

    stopTransition();

    mTransition.to = to;
    mTransition.from = from;
    mTransition.stage = 0.0;
    mTransition.started = true;

    mTimer->start(20);
}

void MainWindow::drawBackground (QSharedPointer<Background>& background, const QRect& rect, float alpha)
{
    if (background == nullptr) {
        return;
    }

    QPainter painter (this);
    painter.setOpacity (alpha);

    switch (background->type) {
        case BACKGROUND_IMAGE: {
            QPixmap *pixmap = getBackground(background->image, rect);
            if(pixmap->isNull()) {
                QString color = mConfiguration->getValue("background-color").toString();
                QColor cor;
                if(!color.isEmpty()) {
                    cor = color;
                } else {
                    cor = "#035290";
                }
                painter.setBrush(cor);
                painter.drawRect(rect);
            } else {
                painter.drawPixmap(rect, *pixmap);
            }
            break;
        }
        case BACKGROUND_COLOR: {
            painter.setBrush(background->color);
            painter.drawRect(rect);
            break;
        }
    }
}

static QPixmap* blurPixmap (QPixmap *pixmap)
{
    QPainter painter (pixmap);
    QImage srcImg = pixmap->toImage ();
    qt_blurImage (&painter, srcImg, BLUR_RADIUS, false, false);
    painter.end ();

    return pixmap;
}
