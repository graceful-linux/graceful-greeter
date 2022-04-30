//
// Created by dingjing on 4/5/22.
//

#include "session-window.h"

#include <QLabel>
#include <QDebug>
#include <QPainter>
#include <QListWidget>
#include <QPushButton>
#include <QMouseEvent>

#include "../common/global.h"
#include "../model/sessions-model.h"

#define ICON_SIZE                   18
#define PROMPT_LEFT                 150
#define LISTWIDGET_SAPCE            3
#define ITEM_WIDTH                  300
#define ITEM_HEIGHT                 40
#define DISPLAY_ROWS                3  //只显示3行，可以通过上下键移动焦点
#define LISTWIDGET_HEIGHT           (DISPLAY_ROWS * (LISTWIDGET_SAPCE * 2 + ITEM_HEIGHT))

SessionWindow::SessionWindow(QAbstractItemModel *model, QWidget *parent) : QMenu(parent), mSessionsModel(model)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setSessionModel(model);
    setObjectName(QStringLiteral("SessionWnd"));
    connect(this, &QMenu::triggered, this, &SessionWindow::onSessionMenuTrigged);

    this->installEventFilter(this);
}

void SessionWindow::setSessionModel(QAbstractItemModel *model)
{
    if(model == nullptr) {
        return ;
    }
    mSessionsModel = model;

    addSessionLabels();
}

void SessionWindow::addSessionLabels()
{
    QSet<QString> filter;
    int sessionCount = mSessionsModel->rowCount ();
    for (int i = 0; i < sessionCount; ++i) {
        QString sessionKey = mSessionsModel->data (mSessionsModel->index (i, 0), SessionsModel::KeyRole).toString();
        if (!filter.contains (sessionKey)) {
            QString sessionName = mSessionsModel->index(i, 0).data(Qt::DisplayRole).toString();
            auto action = new QAction(QIcon(getSessionIcon(sessionKey)), sessionName, this);
            action->setData(sessionKey);
            addAction(action);
            adjustSize();
            filter += sessionKey;
        }
    }
}

void SessionWindow::setCurrentSession(const QString &session)
{
    if (session.isNull() || session.isEmpty() || session == "") {
        return;
    }

    for (auto action : this->actions()) {
        if (action->data().toString()== session) {
            this->setActiveAction(action);
            break;
        }
    }
}

void SessionWindow::onSessionMenuTrigged(QAction *action)
{
    QString sessionKey  = action->data().toString();
    Q_EMIT sessionChanged (sessionKey);
}


QString SessionWindow::getSessionIcon(const QString &session)
{
    QString sessionPrefix = session.left(session.indexOf('-'));

    QString sessionIcon = QString(":/badges/badges/%1_badge.svg").arg(sessionPrefix.toLower());
    QFile iconFile(sessionIcon);
    if (!iconFile.exists()) {
        sessionIcon = QString(":/badges/badges/unknown_badge.svg");
    }
    return sessionIcon;
}

bool SessionWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == 23) {
        auto par = (QWidget*)this->parent();
        par->update();
        close();
    }

    return false;
}
