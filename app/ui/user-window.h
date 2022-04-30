//
// Created by dingjing on 4/11/22.
//

#ifndef GRACEFUL_GREETER_USER_WINDOW_H
#define GRACEFUL_GREETER_USER_WINDOW_H

#include <QMenu>
#include <QWidget>
#include <QSharedPointer>
#include <QAbstractItemModel>

class QLabel;
class QListWidget;
class QPushButton;
class QListWidgetItem;

class UserWindow  : public QMenu
{
    Q_OBJECT
public:
    explicit UserWindow (QAbstractItemModel *model, QWidget *parent = 0);
    void setUserModel (QAbstractItemModel *model);
    void setCurrentUser (const QString &session);

protected:
    bool eventFilter (QObject *obj, QEvent *event) override;

Q_SIGNALS:
    void userChanged (const QString &session);

private Q_SLOTS:
    void onUserMenuClicked (QAction *action);

private:
    void addUserLabels ();

    QString                                 mDefaultUser;
    QAbstractItemModel                     *mUsersModel{};

    QLabel                                 *mPrompt{};
    QListWidget                            *mUsersList{};

};


#endif //GRACEFUL_GREETER_USER_WINDOW_H
