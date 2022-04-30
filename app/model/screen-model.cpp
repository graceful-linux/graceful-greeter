//
// Created by dingjing on 4/2/22.
//

#include "screen-model.h"

#include <QApplication>
#include <QDesktopWidget>

ScreenModel::ScreenModel (QObject *parent) : QAbstractListModel(parent), mDesktop(QApplication::desktop())
{
    loadScreens();

    connect (mDesktop, &QDesktopWidget::resized, this, &ScreenModel::onScreenResized);
    connect (mDesktop, &QDesktopWidget::workAreaResized, this, &ScreenModel::onScreenResized);
    connect (mDesktop, &QDesktopWidget::screenCountChanged, this, &ScreenModel::onScreenCountChanged);
}

int ScreenModel::rowCount (const QModelIndex &parent) const
{
    if (QModelIndex() == parent) {
        mScreen.size();
    }

    return 0;
}

void ScreenModel::loadScreens ()
{
    beginResetModel();

    mScreen.clear();
    for(int i = 0; i < mDesktop->screenCount(); i++) {
        mScreen.append(mDesktop->screenGeometry(i));
    }

    endResetModel();
}

void ScreenModel::onScreenResized (int screen)
{
    if(screen >= 0 && screen < mScreen.size()) {
        mScreen[screen] = mDesktop->screenGeometry(screen);
    }

    QModelIndex index = createIndex(screen, 0);

    Q_EMIT dataChanged(index, index);
}

void ScreenModel::onScreenCountChanged (int newCount)
{
    Q_UNUSED(newCount)

    loadScreens();
}

QVariant ScreenModel::data (const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int row = index.row();

    if (row < mScreen.size() && Qt::UserRole == role) {
        return mScreen[row];
    }

    return QVariant ();
}
