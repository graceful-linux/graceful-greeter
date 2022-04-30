//
// Created by dingjing on 4/2/22.
//

#ifndef GRACEFUL_GREETER_USERS_MODEL_H
#define GRACEFUL_GREETER_USERS_MODEL_H

#include <QtCore/QString>
#include <QAbstractListModel>
#include <QtCore/QSharedDataPointer>

class SecurityUser;
class UsersModelPrivate;

class UsersModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(UserModelRoles)
    Q_PROPERTY (bool mShowGuest READ showGuest WRITE setShowGuest)
    Q_PROPERTY (bool mShowManualLogin READ showManualLogin WRITE setShowManualLogin)
public:
    enum UserModelRoles
    {
        NameRole = Qt::UserRole,
        RealNameRole,
        LoggedInRole,
        BackgroundRole,
        SessionRole,
        HasMessagesRole,
        ImagePathRole,
        BackgroundPathRole,
        UidRole,
        IsLockedRole
    };

public:
    explicit UsersModel (bool hideUsers = false, QObject* parent = nullptr);
    ~UsersModel() override;

    int rowCount () const;
    int rowCount (const QModelIndex &parent) const override;

    QString getDefaultUser ();
    bool userIsValid (const QString&);

    void setCurrentUser (const QString& user);
    QString getUserFaceByName (const QString& name);

    QHash <int, QByteArray> roleNames () const override;
    QVariant data (const QModelIndex &index, int role) const override;

    bool showGuest () const;
    void setShowGuest (bool);
    bool showManualLogin () const;
    void setShowManualLogin (bool);

private:
    int getSecUserCount ();

private:
    bool                    mShowGuest;
    bool                    mShowManualLogin{};
    SecurityUser           *mSecurityUser;
    UsersModelPrivate      *const d_ptr;

    Q_DECLARE_PRIVATE (UsersModel)
};

#endif //GRACEFUL_GREETER_USERS_MODEL_H
