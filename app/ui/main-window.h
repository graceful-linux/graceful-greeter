//
// Created by dingjing on 4/4/22.
//

#ifndef GRACEFUL_GREETER_MAIN_WINDOW_H
#define GRACEFUL_GREETER_MAIN_WINDOW_H

#include <QWidget>
#include <QSharedPointer>
#include <QAbstractNativeEventFilter>
#include <QtConcurrent/QtConcurrentRun>

#include "../model/screen-model.h"

enum BackgroundMode
{
    DRAW_USER_BACKGROUND,
    DRAW_BACKGROUND,
    DRAW_COLOR,
    DRAW_DEFAULT
};

enum BackgroundType
{
    BACKGROUND_IMAGE,
    BACKGROUND_COLOR
};

struct Background
{
    BackgroundType type;
    QColor color;
    QString image;
};

struct Transition
{
    QSharedPointer<Background>  from;
    QSharedPointer<Background>  to;
    float       stage;
    bool        started;
};

class QTimer;
class GreeterWindow;
class Configuration;
class MonitorWatcher;

class MainWindow : public QWidget, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit MainWindow (QWidget *parent = nullptr);
    void setBackground (QSharedPointer<Background>&);

protected:
    void paintEvent (QPaintEvent *) override;
    void mouseMoveEvent (QMouseEvent *) override;
    bool nativeEventFilter (const QByteArray &eventType, void *message, long *result) override;
    static QString getDefaultSession ();

Q_SIGNALS:
    void activeScreenChanged (const QRect& rect);

private Q_SLOTS:
    void onTransition ();
    void onScreenResized ();
    void screenCountEvent ();
    void RRScreenChangeEvent ();
    void onScreenCountChanged (int newCount);

private:
    static void showLater ();
    void stopTransition ();
    void drawTransitionAlpha (const QRect &rect);
    void moveToScreen (QScreen *screen = nullptr);
    QPixmap* getBackground (const QString &path, const QRect &rect);
    void startTransition (QSharedPointer<Background> &, QSharedPointer<Background> &);
    void drawBackground (QSharedPointer<Background> &, const QRect &, float alpha = 1.0);

private:
    ScreenModel                                    *mScreenModel;
    GreeterWindow                                  *mGreeterWnd;
    Configuration                                  *mConfiguration;
    QScreen                                        *mActiveScreen;
    bool                                            mDrawUserBackground = false;
    QString                                         mDefaultBackgroundPath;
    QString                                         mBackgroundPath;
    QString                                         mBackgroundColor;
    QString                                         mLastBackgroundPath;
    QPixmap                                         mCof;
    QPixmap                                         mLogo;
    QPixmap                                        *mPixmap = nullptr;
    QMap<QPair<QString, QString>, QPixmap*>         mBackgrounds;

    static bool                                     mFirst;
    QTimer                                         *mTimer = nullptr;

    QSharedPointer<Background>                      mBackground;
    Transition                                      mTransition;
    BackgroundMode                                  mBackgroundMode;
    QFuture<void>                                   mFuture;
    int                                             mRREventBase = 0;
    int                                             mRRErrorBase = 0;
    int                                             mMonitorCount = 0;
};


#endif //GRACEFUL_GREETER_MAIN_WINDOW_H
