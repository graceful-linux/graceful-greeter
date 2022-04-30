//
// Created by dingjing on 4/10/22.
//

#ifndef GRACEFUL_GREETER_SESSIONS_MODEL_H
#define GRACEFUL_GREETER_SESSIONS_MODEL_H

#include <QIcon>
#include <QtCore/QAbstractListModel>

class SessionsModelPrivate;

class SessionsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(SessionModelRoles SessionType)
public:
    enum SessionModelRoles
    {
        KeyRole = Qt::UserRole,
        IdRole = KeyRole,
        TypeRole
    };

    enum SessionType
    {
        LocalSessions,
        RemoteSessions
    };

    explicit SessionsModel(QObject *parent = nullptr);
    explicit SessionsModel(SessionsModel::SessionType, QObject *parent = nullptr);
    ~SessionsModel() override;

    QString getCurrentSession ();
    void setCurrentSession (const QString& session);
    static QString getSessionIconPath (const QString& iconName);

    QHash<int, QByteArray> roleNames() const override;

    int rowCount () const;
    int rowCount (const QModelIndex &parent) const override;

    QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    SessionsModelPrivate               *d_ptr;

private:
    Q_DECLARE_PRIVATE(SessionsModel)
};


#endif //GRACEFUL_GREETER_SESSIONS_MODEL_H
