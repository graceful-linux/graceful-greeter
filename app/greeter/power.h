//
// Created by dingjing on 4/11/22.
//

#ifndef GRACEFUL_GREETER_POWER_H
#define GRACEFUL_GREETER_POWER_H
#include <QObject>

class Power : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(bool canSuspend READ canSuspend() CONSTANT)
    Q_PROPERTY(bool canHibernate READ canHibernate() CONSTANT)
    Q_PROPERTY(bool canShutdown READ canShutdown() CONSTANT)
    Q_PROPERTY(bool canRestart READ canRestart() CONSTANT)

    Power (QObject *parent=0);
    virtual ~Power ();

    static bool canRestart ();
    static bool canSuspend ();
    static bool canShutdown ();
    static bool canHibernate ();

public Q_SLOTS:
    static bool suspend();
    static bool restart();
    static bool shutdown();
    static bool hibernate();

private:
    class                   PowerPrivate;
    PowerPrivate           *const d;

};


#endif //GRACEFUL_GREETER_POWER_H
