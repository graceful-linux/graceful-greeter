//
// Created by dingjing on 4/5/22.
//

#include <QException>
#include <QVBoxLayout>
#include "power-manager.h"

PowerManager::PowerManager (QWidget *parent) :
    QListWidget (parent),
    mPower (new Power(this)),
    mLastTime(QTime::currentTime())
{
    if (mPower->canSuspend() && mPower->canHibernate()) {
        resize (ITEM_WIDTH * 5, ITEM_HEIGHT);
    } else if (mPower->canSuspend() || mPower->canHibernate()) {
        resize (ITEM_WIDTH * 4, ITEM_HEIGHT);
    } else {
        resize (ITEM_WIDTH * 3, ITEM_HEIGHT);
    }

    setFlow (QListWidget::LeftToRight);
    setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    setSelectionMode (QListWidget::NoSelection);

    QObject::connect (this, SIGNAL(itemClicked (QListWidgetItem*)), this, SLOT(powerClicked (QListWidgetItem*)));

    mRebootWidget = new QWidget (this);
    mRebootWidget->setObjectName ("rebootWidget");
    auto rebootFace = new QLabel (this);
    mRebootLabel = new QLabel (this);
    rebootFace->setAlignment (Qt::AlignCenter);
    mRebootLabel->setAlignment (Qt::AlignCenter);
    rebootFace->setPixmap (QPixmap(":/images/images/reboot.png").scaled(58,58));
    mRebootLabel->setText (tr("Restart"));
    mRebootWidget->setFixedSize (ITEM_WIDTH,ITEM_HEIGHT);
    auto rebootLayout = new QVBoxLayout (mRebootWidget);
    rebootLayout->addWidget (rebootFace);
    rebootLayout->addWidget (mRebootLabel);
#if DEBUG
    mRebootWidget->setStyleSheet (QString::fromUtf8 ("border:1px solid blue"));
#endif

    mShutdownWidget = new QWidget (this);
    mShutdownWidget->setObjectName ("shutdownWidget");
    auto shutdownFace  = new QLabel (this);
    mShutdownLabel = new QLabel (this);
    mShutdownLabel->setAlignment (Qt::AlignCenter);
    shutdownFace->setAlignment (Qt::AlignCenter);
    shutdownFace->setPixmap (QPixmap(":/images/images/shutdown.png").scaled(58,58));
    mShutdownLabel->setText (tr("Power Off"));
    mShutdownWidget->setFixedSize (ITEM_WIDTH,ITEM_HEIGHT);
    auto shutdownLayout = new QVBoxLayout (mShutdownWidget);
    shutdownLayout->addWidget (shutdownFace);
    shutdownLayout->addWidget (mShutdownLabel);
#if DEBUG
    mShutdownWidget->setStyleSheet (QString::fromUtf8 ("border:1px solid blue"));
#endif

    if (mPower->canSuspend()){
        mSuspendWidget = new QWidget (this);
        mSuspendWidget->setObjectName ("suspendWidget");
        auto suspendFace = new QLabel (this);
        auto suspendLabel = new QLabel (this);
        suspendFace->setAlignment (Qt::AlignCenter);
        suspendLabel->setAlignment (Qt::AlignCenter);
        suspendFace->setPixmap (QPixmap(":/images/images/suspend.png").scaled(48,48));
        suspendLabel->setText (tr("Suspend"));
        mSuspendWidget->setFixedSize (ITEM_WIDTH,ITEM_HEIGHT);
        auto suspendLayout = new QVBoxLayout (mSuspendWidget);
        suspendLayout->addWidget(suspendFace);
        suspendLayout->addWidget(suspendLabel);
#if DEBUG
        mSuspendWidget->setStyleSheet (QString::fromUtf8 ("border:1px solid blue"));
#endif
    }

    if (mPower->canHibernate()) {
        mHibernateWidget = new QWidget(this);
        mHibernateWidget->setObjectName("suspendWidget");
        auto hibernateFace = new QLabel(this);
        auto hibernateLabel = new QLabel(this);
        hibernateFace->setAlignment(Qt::AlignCenter);
        hibernateLabel->setAlignment(Qt::AlignCenter);
        hibernateFace->setPixmap(QPixmap(":/images/images/hibernate.png").scaled(48,48));
        hibernateLabel->setText(tr("Sleep"));
        mHibernateWidget->setFixedSize(ITEM_WIDTH,ITEM_HEIGHT);
        auto hibernateLayout = new QVBoxLayout (mHibernateWidget);
        hibernateLayout->addWidget(hibernateFace);
        hibernateLayout->addWidget(hibernateLabel);
#if DEBUG
        mHibernateWidget->setStyleSheet (QString::fromUtf8 ("border:1px solid blue"));
#endif
    }

    auto item1 = new QListWidgetItem();
    item1->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    insertItem(0, item1);
    setItemWidget(item1, mRebootWidget);

    auto item2 = new QListWidgetItem();
    item2->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    insertItem(1, item2);
    setItemWidget(item2, mShutdownWidget);

    if (mPower->canSuspend()) {
        auto item3 = new QListWidgetItem();
        item3->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        insertItem(2, item3);
        setItemWidget(item3, mSuspendWidget);
    }

    if (mPower->canHibernate()) {
        auto item = new QListWidgetItem();
        item->setSizeHint (QSize(ITEM_WIDTH, ITEM_HEIGHT));
        if (mPower->canSuspend()) {
            insertItem (3, item);
        } else {
            insertItem (2, item);
        }
        setItemWidget (item, mHibernateWidget);
    }
    adjustSize();
}

QSize PowerManager::windowSize ()
{
    if (mPower->canSuspend() && mPower->canHibernate()) {
        return {ITEM_WIDTH * 4, ITEM_HEIGHT};
    } else if (mPower->canSuspend() || mPower->canHibernate()) {
        return {ITEM_WIDTH * 3, ITEM_HEIGHT};
    } else {
        return {ITEM_WIDTH * 2, ITEM_HEIGHT};
    }
}

void PowerManager::refreshTranslate ()
{
    mRebootLabel->setText (tr("Restart"));
    mShutdownLabel->setText (tr("Power Off"));
    mSwitchLabel->setText (tr("Switch User"));
}

void PowerManager::lockWidgetClicked ()
{

}

void PowerManager::switchWidgetClicked ()
{
    Q_EMIT switchToUser ();
}

void PowerManager::suspendWidgetClicked ()
{
    try {
        Q_EMIT switchToUser ();
        mPower->suspend();
    } catch (QException &e) {
    }
}

void PowerManager::rebootWidgetClicked ()
{
    try {
        mPower->restart ();
        close();
    } catch (QException &e) {
    }
}

void PowerManager::shutdownWidgetClicked ()
{
    try {
        mPower->shutdown();
        close();
    } catch (QException &e) {

    }
}

void PowerManager::hibernateWidgetClicked ()
{
    try {
        Q_EMIT switchToUser ();
        mPower->hibernate ();
    } catch (QException &e) {
    }
}

void PowerManager::powerClicked (QListWidgetItem *item)
{
    int interval = mLastTime.msecsTo (QTime::currentTime());
    if (interval < 200 && interval > -200) {
        return;
    }

    mLastTime = QTime::currentTime ();

    int x = row(item);
    switch (x) {
        case 0:
            switchWidgetClicked();
            break;
        case 1:
            rebootWidgetClicked();
            break;
        case 2:
            shutdownWidgetClicked();
            break;
        case 3:
            if (mPower->canSuspend()) {
                suspendWidgetClicked();
            } else if (mPower->canHibernate()) {
                hibernateWidgetClicked();
            }
            break;
        case 4:
            if (mPower->canHibernate()) {
                hibernateWidgetClicked();
            }
        default:
            break;
    }
}
