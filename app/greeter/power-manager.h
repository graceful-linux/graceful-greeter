//
// Created by dingjing on 4/5/22.
//

#ifndef GRACEFUL_GREETER_POWERMANAGER_H
#define GRACEFUL_GREETER_POWERMANAGER_H
#include <QSize>
#include <QTime>
#include <QLabel>
#include <QWidget>
#include <QListWidget>

#include "power.h"

#define ITEM_WIDTH              168
#define ITEM_HEIGHT             ITEM_WIDTH
#define ITEM_SPACING            (ITEM_WIDTH/8)

class QListWidget;
class QListWidgetItem;

class PowerManager : public QListWidget
{
    Q_OBJECT
public:
    explicit PowerManager (QWidget* parent = nullptr);

    QSize windowSize ();

Q_SIGNALS:
    void lock ();
    void switchToUser ();

private:
    void refreshTranslate ();
    void lockWidgetClicked ();
    void switchWidgetClicked ();
    void suspendWidgetClicked ();
    void rebootWidgetClicked ();
    void shutdownWidgetClicked ();
    void hibernateWidgetClicked ();

private Q_SLOTS:
    void powerClicked (QListWidgetItem* item);

private:
    QListWidget                    *mList{};
    QWidget                        *mLockWidget{};
    QWidget                        *mSuspendWidget{};
    QWidget                        *mRebootWidget{};
    QWidget                        *mShutdownWidget{};
    QWidget                        *mHibernateWidget{};
    QLabel                         *mSwitchLabel{};
    QLabel                         *mRebootLabel{};
    QLabel                         *mShutdownLabel{};
    Power                          *mPower;
    QTime                           mLastTime;
};


#endif //GRACEFUL_GREETER_POWERMANAGER_H
