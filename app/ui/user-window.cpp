//
// Created by dingjing on 4/11/22.
//
#include "user-window.h"

#include "../model/users-model.h"

#include <QFile>
#include <QEvent>
#include <QDebug>


UserWindow::UserWindow (QAbstractItemModel *model, QWidget *parent) : QMenu(parent), mUsersModel(model)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setUserModel(model);
    setObjectName(QStringLiteral("SessionWnd"));
    connect(this, &QMenu::triggered, this, &UserWindow::onUserMenuClicked);

    this->installEventFilter(this);
}

void UserWindow::setUserModel (QAbstractItemModel *model)
{
    if (!model) return;

    mUsersModel = model;

    addUserLabels();
}

void UserWindow::setCurrentUser (const QString& user)
{
    if (user.isNull() || user.isEmpty()) {
        return;
    }

    for (auto action : this->actions()) {
        qDebug () << user << " -- " << action->data().toString();
        if (action->data().toString() == user) {
            this->setActiveAction(action);
            break;
        }
    }
}

bool UserWindow::eventFilter (QObject *obj, QEvent *event)
{
    if (event->type() == 23) {
        auto par = (QWidget*)this->parent();
        par->update();
        close();
    }

    return false;
}

void UserWindow::onUserMenuClicked (QAction *action)
{
    QString userKey  = action->data().toString();
    Q_EMIT userChanged (userKey);
}

void UserWindow::addUserLabels ()
{
    if (!mUsersModel) {
        qCritical() << "users model is null!";
        return;
    }

    for (int i = 0; i < mUsersModel->rowCount(); ++i) {
        QString userName = mUsersModel->data (mUsersModel->index (i, 0), UsersModel::NameRole).toString();
        QString userKey = mUsersModel->index(i, 0).data(Qt::UserRole).toString();
        auto action = new QAction(QIcon(reinterpret_cast<UsersModel*>(mUsersModel)->getUserFaceByName(userName)), userName, this);
        action->setData(userName);
        addAction(action);
        adjustSize();
    }
}

