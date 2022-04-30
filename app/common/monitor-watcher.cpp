//
// Created by dingjing on 4/3/22.
//

#include <QProcess>
#include <QStandardPaths>
#include "monitor-watcher.h"

MonitorWatcher::MonitorWatcher (QObject *parent) : QThread (parent),
    mMonitorFirst(""), mVirtualSize(0, 0), mMonitorCount(0)
{

}

MonitorWatcher::~MonitorWatcher ()
{
    requestInterruption ();
    terminate ();
    wait ();
}

QSize MonitorWatcher::getVirtualSize ()
{
    return mVirtualSize;
}

int MonitorWatcher::getMonitorCount () const
{
    return mMonitorCount;
}

void MonitorWatcher::run ()
{
    /* 每隔3秒遍历一次显卡接口的连接状态 */
    while (!isInterruptionRequested()) {
        getMonitors();
        int tmp = mMonitors.keys().size();
        if (tmp == 1 && mMonitorCount == 1){
            if (mMonitorFirst != mMonitorNames[0]) {
                mMonitorFirst = mMonitorNames[0];
                Q_EMIT monitorCountChanged (mMonitorCount);
            }
        } else if (tmp != mMonitorCount) {
            if (mMonitorFirst == "") {
                mMonitorFirst = mMonitorNames[0];
            }
            mMonitorCount = tmp;
            Q_EMIT monitorCountChanged (mMonitorCount);
        }
        sleep(3);
    }
}

void MonitorWatcher::getMonitors ()
{
    QProcess subProcess;
    subProcess.setProgram(QStandardPaths::findExecutable("xrandr"));
    subProcess.setArguments({"-q"});
    subProcess.start(QIODevice::ReadOnly);
    subProcess.waitForFinished();
    QString outputs = subProcess.readAll();
    QStringList lines = outputs.split('\n');
    QString name;
    QVector<QString> modes;
    bool find = false;
    QString lastName;
    mMonitorNames.clear();
    mMonitors.clear();
    for (auto & line : lines) {
        if (line.indexOf(" connected") != -1) {   //找显示器名称
            name = line.left (line.indexOf(' '));
            mMonitorNames.push_back(name);
            if (find) {    //又找到了一个显示器，将上一个显示器的信息存到map
                mMonitors [lastName] = modes;
            }
            find = true;
            lastName = name;
            modes.clear();
        } else {
            if (line.startsWith(' ')) {      //获取分辨率
                QString mode = line.trimmed().split(' ').at(0);
                modes.push_back(mode);
            }
        }
    }
    if (!name.isEmpty()) {
        mMonitors[name] = modes;     //将最后一个显示器的信息存储到map
    }
}

QSize MonitorWatcher::getMonitorMaxSize (const QString &drm)
{
    Q_UNUSED (drm)
    return mVirtualSize;
}
