//
// Created by dingjing on 4/2/22.
//

#include "users-model.h"

#include "../greeter/security-user.h"

#include <QtGui/QIcon>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QString>

#include <lightdm.h>
#include <glib-object.h>

#include <exception>
#include <QStandardItem>

class UsersModelPrivate;

class UserItem
{
public:
    QString displayName() const;

public:
    QString             name;
    QString             realName;
    QString             homeDirectory;
    QString             image;
    QString             background;
    QString             session;
    bool                isLoggedIn{};
    bool                hasMessages{};
    quint64             uid{};
    bool                isLocked{};
};

QString UserItem::displayName() const
{
    if (realName.isEmpty()) {
        return name;
    } else {
        return realName;
    }
}

class UsersModelPrivate
{
public:
    explicit UsersModelPrivate (UsersModel *parent);
    virtual ~UsersModelPrivate();

protected:
    UsersModel                 *const q_ptr;
    QList<UserItem>             users;

    QString                     mCurrentUser;

    void loadUsers();

    static void cb_userAdded (LightDMUserList *userList, LightDMUser *user, gpointer data);
    static void cb_userChanged (LightDMUserList *userList, LightDMUser *user, gpointer data);
    static void cb_userRemoved (LightDMUserList *userList, LightDMUser *user, gpointer data);
private:
    Q_DECLARE_PUBLIC(UsersModel)
};

UsersModelPrivate::UsersModelPrivate (UsersModel* parent) : q_ptr(parent)
{
#if !defined(GLIB_VERSION_2_36)
    g_type_init();
#endif
}

UsersModelPrivate::~UsersModelPrivate ()
{
    g_signal_handlers_disconnect_by_data (lightdm_user_list_get_instance(), this);
}

void UsersModelPrivate::loadUsers()
{
    Q_Q(UsersModel);

    int rowCount = lightdm_user_list_get_length(lightdm_user_list_get_instance());

    if (rowCount == 0) {
        return;
    } else {
        q->beginInsertRows(QModelIndex(), 0, rowCount-1);

        const GList *items, *item;
        items = lightdm_user_list_get_users(lightdm_user_list_get_instance());
        for (item = items; item; item = item->next) {
            auto ldmUser = static_cast<LightDMUser*>(item->data);

            UserItem user;
            user.name = QString::fromUtf8(lightdm_user_get_name(ldmUser));
            user.homeDirectory = QString::fromUtf8(lightdm_user_get_home_directory(ldmUser));
            user.realName = QString::fromUtf8(lightdm_user_get_real_name(ldmUser));
            user.image = QString::fromUtf8(lightdm_user_get_image(ldmUser));
            user.background = QString::fromUtf8(lightdm_user_get_background(ldmUser));
            user.session = QString::fromUtf8(lightdm_user_get_session(ldmUser));
            user.isLoggedIn = lightdm_user_get_logged_in(ldmUser);
            user.hasMessages = lightdm_user_get_has_messages(ldmUser);
            user.uid = (quint64)lightdm_user_get_uid(ldmUser);
            user.isLocked = lightdm_user_get_is_locked(ldmUser);
            users.append(user);
        }

        q->endInsertRows();
    }
    g_signal_connect(lightdm_user_list_get_instance(), LIGHTDM_USER_LIST_SIGNAL_USER_ADDED, G_CALLBACK (cb_userAdded), this);
    g_signal_connect(lightdm_user_list_get_instance(), LIGHTDM_USER_LIST_SIGNAL_USER_CHANGED, G_CALLBACK (cb_userChanged), this);
    g_signal_connect(lightdm_user_list_get_instance(), LIGHTDM_USER_LIST_SIGNAL_USER_REMOVED, G_CALLBACK (cb_userRemoved), this);
}

void UsersModelPrivate::cb_userAdded(LightDMUserList* userList, LightDMUser *ldmUser, gpointer data)
{
    Q_UNUSED (userList)
    auto that = static_cast<UsersModelPrivate*>(data);

    that->q_func()->beginInsertRows(QModelIndex(), that->users.size(), that->users.size());

    UserItem user;
    user.name = QString::fromUtf8(lightdm_user_get_name(ldmUser));
    user.homeDirectory = QString::fromUtf8(lightdm_user_get_home_directory(ldmUser));
    user.realName = QString::fromUtf8(lightdm_user_get_real_name(ldmUser));
    user.image = QString::fromUtf8(lightdm_user_get_image(ldmUser));
    user.background = QString::fromUtf8(lightdm_user_get_background(ldmUser));
    user.isLoggedIn = lightdm_user_get_logged_in(ldmUser);
    user.hasMessages = lightdm_user_get_has_messages(ldmUser);
    user.uid = (quint64)lightdm_user_get_uid(ldmUser);
    user.isLocked = lightdm_user_get_is_locked(ldmUser);
    that->users.append(user);

    that->q_func()->endInsertRows();
}

void UsersModelPrivate::cb_userChanged (LightDMUserList* userList, LightDMUser *ldmUser, gpointer data)
{
    Q_UNUSED(userList)
    auto that = static_cast<UsersModelPrivate*>(data);

    QString userToChange = QString::fromUtf8(lightdm_user_get_name(ldmUser));

    for (int i=0;i<that->users.size();i++) {
        if (that->users[i].name == userToChange) {

            that->users[i].homeDirectory = QString::fromUtf8(lightdm_user_get_home_directory(ldmUser));
            that->users[i].realName = QString::fromUtf8(lightdm_user_get_real_name(ldmUser));
            that->users[i].image = QString::fromUtf8(lightdm_user_get_image(ldmUser));
            that->users[i].background = QString::fromUtf8(lightdm_user_get_background(ldmUser));
            that->users[i].isLoggedIn = lightdm_user_get_logged_in(ldmUser);
            that->users[i].hasMessages = lightdm_user_get_has_messages(ldmUser);
            that->users[i].uid = (quint64)lightdm_user_get_uid(ldmUser);
            that->users[i].isLocked = lightdm_user_get_is_locked(ldmUser);

            QModelIndex index = that->q_ptr->createIndex(i, 0);
            that->q_ptr->dataChanged(index, index);
            break;
        }
    }
}

void UsersModelPrivate::cb_userRemoved(LightDMUserList *userList, LightDMUser *ldmUser, gpointer data)
{
    Q_UNUSED(userList)

    auto that = static_cast<UsersModelPrivate*>(data);
    QString userToRemove = QString::fromUtf8(lightdm_user_get_name(ldmUser));

    for (int i=0;i<that->users.size();i++) {
        if (that->users[i].name == userToRemove) {
            that->q_ptr->beginRemoveRows(QModelIndex(), i, i);
            that->users.removeAt(i);
            that->q_ptr->endRemoveRows();
            break;
        }
    }
}

UsersModel::UsersModel (bool hideUsers, QObject *parent) :
    QAbstractListModel(parent),
    mShowGuest (false),
    mSecurityUser (SecurityUser::instance()),
    d_ptr(new UsersModelPrivate(this))
{
    Q_D(UsersModel);
    d->loadUsers();

#if DEBUG
    qDebug() << "user count:" << rowCount() << " sec user count:" << getSecUserCount();
#endif

    if (!hideUsers) {
        if (getSecUserCount() == 0) {
            setShowManualLogin (true);
        }
    } else {
        setShowManualLogin (true);
    }
}

UsersModel::~UsersModel ()
{
    delete d_ptr;
}

QHash<int, QByteArray> UsersModel::roleNames () const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[Qt::DecorationRole] = "decoration";
    roles[NameRole] = "name";
    roles[RealNameRole] = "realName";
    roles[LoggedInRole] = "loggedIn";
    roles[BackgroundRole] = "background";
    roles[BackgroundPathRole] = "backgroundPath";
    roles[SessionRole] = "session";
    roles[HasMessagesRole] = "hasMessages";
    roles[ImagePathRole] = "imagePath";
    roles[UidRole] = "uid";
    roles[IsLockedRole] = "isLocked";

    return roles;
}

int UsersModel::rowCount (const QModelIndex &parent) const
{
    Q_D (const UsersModel);
    if (parent == QModelIndex()) {
        return d->users.size();
    }

    return 0;
}

int UsersModel::rowCount () const
{
    return rowCount (QModelIndex());
}

QString UsersModel::getDefaultUser ()
{
    Q_D(UsersModel);

    int userNum = rowCount();
    if (userNum <= 0) {
        qCritical() << "user count is 0";
        return nullptr;
    }

    if (userIsValid (d->mCurrentUser)) {
        return d->mCurrentUser;
    }

    for (int i = 0; i < userNum; ++i) {
        QString uid = data (index (i, 0), UsersModel::UidRole).toString();
        if ("1000" == uid) {
            QString name = data (index (i, 0), UsersModel::NameRole).toString();
            return name;
        }
    }

    return data (index (0, 0), UsersModel::NameRole).toString();
}

QVariant UsersModel::data (const QModelIndex &index, int role) const
{
    Q_D(const UsersModel);

    if (!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    switch (role) {
        case Qt::DisplayRole:
            return d->users[row].displayName();
        case Qt::DecorationRole:
            return QIcon(d->users[row].image);
        case UsersModel::NameRole:
            return d->users[row].name;
        case UsersModel::RealNameRole:
            return d->users[row].realName;
        case UsersModel::SessionRole:
            return d->users[row].session;
        case UsersModel::LoggedInRole:
            return d->users[row].isLoggedIn;
        case UsersModel::BackgroundRole:
            return QPixmap(d->users[row].background);
        case UsersModel::BackgroundPathRole:
            return d->users[row].background;
        case UsersModel::HasMessagesRole:
            return d->users[row].hasMessages;
        case UsersModel::ImagePathRole:
            return d->users[row].image;
        case UsersModel::UidRole:
            return d->users[row].uid;
        case UsersModel::IsLockedRole:
            return d->users[row].isLocked;
        default:
            break;
    }

    return QVariant();
}

bool UsersModel::showGuest () const
{
    return mShowGuest;
}

void UsersModel::setShowGuest (bool isShowGuest)
{
    Q_UNUSED(isShowGuest)
#if 0
    if (mShowGuest == isShowGuest)
        return;

    mShowGuest = isShowGuest;
    if (mShowGuest) {
        auto *guest = new QStandardItem(tr("Guest"));
        guest->setData("*guest", UsersModel::NameRole);
        guest->setData(tr("Guest"), UsersModel::RealNameRole);
        extraRowModel()->appendRow(guest);
    } else {
        QStandardItemModel *model = extraRowModel();
        for (int i = 0; i < model->rowCount(); i++) {
            QStandardItem *item = model->item(i, 0);
            if (item->text() == tr("Guest Session")) {
                model->removeRow(i);
            }
        }
    }

#endif
}

bool UsersModel::showManualLogin () const
{
    return mShowManualLogin;
}

void UsersModel::setShowManualLogin (bool isShowManualLogin)
{
    if (mShowManualLogin == isShowManualLogin) {
        return;
    }

    mShowManualLogin = isShowManualLogin;
//    if (mShowManualLogin) {
//        auto *manualLogin = new QStandardItem(tr("Login"));
//        manualLogin->setData("*login", QLightDM::UsersModel::NameRole);
//        manualLogin->setData(tr("Login"), QLightDM::UsersModel::RealNameRole);
//        extraRowModel()->appendRow(manualLogin);
//    } else {
//        QStandardItemModel *model = extraRowModel();
//        for (int i = 0; i < model->rowCount(); i++) {
//            QStandardItem *item = model->item(i, 0);
//            if (item->text() == tr("Login")) {
//                model->removeRow(i);
//            }
//        }
//    }
}

int UsersModel::getSecUserCount ()
{
    int count = 0;
    int rowCountNum = rowCount (QModelIndex());

    for (int i = 0; i < rowCountNum; i++) {
        QString name = this->index(i).data(UsersModel::NameRole).toString();
        if (mSecurityUser->isSecurityUser(name)) {
            ++count;
        }
    }

    return count;
}

QString UsersModel::getUserFaceByName (const QString& name)
{
    int userNum = rowCount();
    QString userFace = ":/images/images/default_face.png";

    if (name.isEmpty() || name.isNull() || "" == name || userNum <= 0) {
        return userFace;
    }

    for (auto i = 0; i < userNum; ++i) {
        QString name1 = data (index (i, 0), UsersModel::NameRole).toString();
        if (name == name1) {
            QString f1 = data (index (i, 0), UsersModel::ImagePathRole).toString();
            if (QFile::exists(f1)) {
                userFace = f1;
            }
            break;
        }
    }

    return userFace;
}

void UsersModel::setCurrentUser (const QString& user)
{
    Q_D (UsersModel);

    d->mCurrentUser = user;
}

bool UsersModel::userIsValid (const QString& user)
{
    if (user.isNull() || user.isEmpty() || "" == user) {
        return false;
    }

    return true;
}




