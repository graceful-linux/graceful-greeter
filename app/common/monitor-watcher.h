//
// Created by dingjing on 4/3/22.
//

#ifndef GRACEFUL_GREETER_MONITORWATCHER_H
#define GRACEFUL_GREETER_MONITORWATCHER_H
#include <QMap>
#include <QSize>
#include <QVector>
#include <QThread>

class MonitorWatcher : public QThread
{
    Q_OBJECT
public:
    explicit MonitorWatcher(QObject *parent=nullptr);
    ~MonitorWatcher() override;

    int getMonitorCount() const;
    QSize getVirtualSize();

Q_SIGNALS:
    void monitorCountChanged (int newCount);
    void virtualSizeChanged (const QSize &newVirtualSize);

protected:
    void run () override;

private:
    void getMonitors ();
    QSize getMonitorMaxSize (const QString &drm);

private:
    QMap<QString, QVector<QString>>     mMonitors;
    QStringList                         mMonitorNames;
    QString                             mMonitorFirst;

    QMap<QString, QString>              mDrmStatus;
    QSize                               mVirtualSize;
    int                                 mMonitorCount;
};


#endif //GRACEFUL_GREETER_MONITORWATCHER_H
