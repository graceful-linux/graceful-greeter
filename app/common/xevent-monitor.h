//
// Created by dingjing on 4/3/22.
//

#ifndef GRACEFUL_GREETER_XEVENT_MONITOR_H
#define GRACEFUL_GREETER_XEVENT_MONITOR_H

#include <QThread>
#include <QMetaMethod>

class XEventMonitorPrivate;
class XEventMonitor : public QThread
{
    Q_OBJECT
public:
    static XEventMonitor *instance ();

private:
    explicit XEventMonitor (QObject *parent = nullptr);
    ~XEventMonitor () override;

Q_SIGNALS:
    void buttonDrag (int x, int y);
    void buttonPress (int x, int y);
    void buttonRelease (int x, int y);

    void keyPress (int keyCode);
    void keyRelease (int keyCode);

    void keyPress (const QString &key);
    void keyRelease (const QString &key);

protected:
    void run () override;

private:
    Q_DECLARE_PRIVATE (XEventMonitor)
    XEventMonitorPrivate           *d_ptr;
    static XEventMonitor           *gInstance;
};

/**
 * @brief 检查大写开关状态
 * @return 开(true)，关(false)
 */
bool checkCapsState ();
bool checkNumLockState ();


#endif //GRACEFUL_GREETER_XEVENT_MONITOR_H
