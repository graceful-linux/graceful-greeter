//
// Created by dingjing on 4/2/22.
//

#ifndef GRACEFUL_GREETER_SCREEN_MODEL_H
#define GRACEFUL_GREETER_SCREEN_MODEL_H
#include <QRect>
#include <QVector>
#include <QAbstractListModel>

class QDesktopWidget;

class ScreenModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ScreenModel (QObject* parent=nullptr);

    int rowCount (const QModelIndex &parent = QModelIndex()) const override;
    QVariant data (const QModelIndex &index, int role = Qt::UserRole) const override;

private Q_SLOTS:
    void loadScreens ();
    void onScreenResized (int screen);
    void onScreenCountChanged (int newCount);

private:
    QVector<QRect>      mScreen;
    QDesktopWidget     *mDesktop;
};


#endif //GRACEFUL_GREETER_SCREEN_MODEL_H
