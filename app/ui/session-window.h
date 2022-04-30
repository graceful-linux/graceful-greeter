//
// Created by dingjing on 4/5/22.
//

#ifndef GRACEFUL_GREETER_SESSION_WINDOW_H
#define GRACEFUL_GREETER_SESSION_WINDOW_H

#include <QMenu>
#include <QWidget>
#include <QSharedPointer>
#include <QAbstractItemModel>

class QLabel;
class IconLabel;
class QListWidget;
class QPushButton;
class QListWidgetItem;
class SessionWindow : public QMenu
{
    Q_OBJECT
public:
    explicit SessionWindow(QAbstractItemModel *model, QWidget *parent = 0);
    void setSessionModel(QAbstractItemModel *model);
    void setCurrentSession(const QString &session);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

Q_SIGNALS:
    void sessionChanged(const QString &session);

private Q_SLOTS:
    void onSessionMenuTrigged(QAction *action);

private:
    void addSessionLabels();
    static QString getSessionIcon(const QString&);

private:
    QString              mDefaultSession;
    QAbstractItemModel  *mSessionsModel;

    //UI
    QLabel              *mPrompt{};
    QListWidget         *mSessionsList{};
};
#endif //GRACEFUL_GREETER_SESSION_WINDOW_H
