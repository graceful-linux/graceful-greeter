//
// Created by dingjing on 4/10/22.
//

#include "sessions-model.h"

#include <QtCore/QVariant>
#include <QtCore/QDebug>

#include <QFile>
#include <lightdm.h>
#include <QModelIndex>

class SessionItem
{
public:
    QString key;
    QString type;
    QString name;
    QString comment;
};

class SessionsModelPrivate
{
public:
    explicit SessionsModelPrivate(SessionsModel *parent);

    void loadSessions(SessionsModel::SessionType sessionType);

protected:
    SessionsModel                          *q_ptr;
    QList<SessionItem>                      mAllSessions;

    QString                                 mCurrentSession = nullptr;

private:
    Q_DECLARE_PUBLIC(SessionsModel)

};

SessionsModelPrivate::SessionsModelPrivate(SessionsModel *parent) : q_ptr(parent)
{
#if !defined(GLIB_VERSION_2_36)
    g_type_init();
#endif
}

void SessionsModelPrivate::loadSessions(SessionsModel::SessionType sessionType)
{
    GList *ldmSessions;

    switch (sessionType) {
        case SessionsModel::RemoteSessions:
            ldmSessions = lightdm_get_remote_sessions();
            break;
        case SessionsModel::LocalSessions:
            /* Fall through*/
        default:
            ldmSessions = lightdm_get_sessions();
            break;
    }

    for (GList* item = ldmSessions; item; item = item->next) {
        auto ldmSession = static_cast<LightDMSession*>(item->data);
        Q_ASSERT(ldmSession);

        SessionItem session;
        session.key = QString::fromUtf8(lightdm_session_get_key(ldmSession));
        session.type = QString::fromUtf8(lightdm_session_get_session_type(ldmSession));
        session.name = QString::fromUtf8(lightdm_session_get_name(ldmSession));
        session.comment = QString::fromUtf8(lightdm_session_get_comment(ldmSession));

#if DEBUG
        qDebug() << "session key: " << session.key << "  session type:" << session.type << " session name: " << session.name;
#endif

        mAllSessions.append(session);
    }
}


//deprecated constructor for ABI compatability.
SessionsModel::SessionsModel(QObject *parent) :
        QAbstractListModel(parent),
        d_ptr(new SessionsModelPrivate(this))
{
    Q_D(SessionsModel);

    d->loadSessions(SessionsModel::LocalSessions);
}

SessionsModel::SessionsModel(SessionsModel::SessionType sessionType, QObject *parent) :
        QAbstractListModel(parent),
        d_ptr(new SessionsModelPrivate(this))
{
    Q_D(SessionsModel);

    d->loadSessions(sessionType);
}

SessionsModel::~SessionsModel()
{
    delete d_ptr;
}

QString SessionsModel::getSessionIconPath (const QString& iconName)
{
    static QSet<QString> badges = {
            "budgie_badge.svg",
            "gnome_badge.svg",
            "graceful-linux_badge.svg",
            "kde_badge.svg",
            "lubuntu_badge.svg",
            "mate_badge.svg",
            "pantheon_badge.svg",
            "plasma_badge.svg",
            "ubuntu_badge.svg",
            "ukui_badge.svg",
            "ukui_wayland_badge.svg",
            "xface_badge.svg",
            "xubuntu_badge.svg",
    };
    QString iconNameFull = QString ("%1_badge.svg").arg (iconName.toLower());
    if (!badges.contains (iconNameFull)) {
        qDebug() << "session icon: " << iconNameFull << " not exists!";
        iconNameFull = "unknown_badge.svg";
    }

    qDebug() << "session: " << iconNameFull;

    return QString(":/badges/badges/%1").arg(iconNameFull);
}

QString SessionsModel::getCurrentSession ()
{
    Q_D(SessionsModel);

    if (rowCount () <= 0) {
        qCritical() << "There is no session installed!";
        return nullptr;
    }

    QString session = nullptr;

    if (!d->mCurrentSession.isNull() && !d->mCurrentSession.isEmpty() && "" != d->mCurrentSession) {
        session = d->mCurrentSession;
    }

    // first element in SessionModel
    if (session == nullptr || session.isNull() || session.isEmpty() || session == "") {
        // SessionsModel::KeyRole:
        session = d->mCurrentSession = data (index (0, 0), SessionsModel::KeyRole).toString();
    }

    qDebug() << "get default session: " << session;

    return session;
}

QHash<int, QByteArray> SessionsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[KeyRole] = "key";
    roles[Qt::DisplayRole] = "display";
    roles[Qt::ToolTipRole] = "toolTip";
    return roles;
}

int SessionsModel::rowCount () const
{
    return rowCount(QModelIndex());
}

int SessionsModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const SessionsModel);

    if (parent == QModelIndex()) { //if top level
        return d->mAllSessions.size();
    } else {
        return 0; // no child elements.
    }
}

QVariant SessionsModel::data(const QModelIndex &index, int role) const
{
    Q_D(const SessionsModel);

    if (!index.isValid()) {
        return QVariant();
    }

    int row = index.row();

    switch (role) {
        case SessionsModel::KeyRole:
            return d->mAllSessions[row].key;
        case SessionsModel::TypeRole:
            return d->mAllSessions[row].type;
        case Qt::DisplayRole:
            return d->mAllSessions[row].name;
        case Qt::ToolTipRole:
            return d->mAllSessions[row].comment;
        default:
            break;
    }

    return QVariant();
}

void SessionsModel::setCurrentSession (const QString &session)
{
    Q_D(SessionsModel);
    if (session.isNull() || session.isEmpty() || "" == session) {
        return;
    }

    d->mCurrentSession = session;
}




