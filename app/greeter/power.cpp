//
// Created by dingjing on 4/11/22.
//

#include "power.h"

#include <lightdm.h>

class Power::PowerPrivate
{
public:
    PowerPrivate();
};

Power::PowerPrivate::PowerPrivate()
= default;


Power::Power(QObject *parent) :
    QObject(parent),
    d(new PowerPrivate)
{
}

Power::~Power()
{
    delete d;
}

bool Power::canSuspend()
{
    return lightdm_get_can_suspend ();
}

bool Power::suspend()
{
    return lightdm_suspend (nullptr);
}

bool Power::canHibernate()
{
    return lightdm_get_can_hibernate ();
}

bool Power::hibernate()
{
    return lightdm_hibernate (nullptr);
}

bool Power::canShutdown()
{
    return lightdm_get_can_shutdown ();
}

bool Power::shutdown()
{
    return lightdm_shutdown (nullptr);
}

bool Power::canRestart()
{
    return lightdm_get_can_restart ();
}

bool Power::restart()
{
    return lightdm_restart (nullptr);
}