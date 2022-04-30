//
// Created by dingjing on 4/2/22.
//

#include "proxy-model.h"

ProxyModel::ProxyModel (QObject *parent) : QAbstractListModel (parent), mModel(nullptr), mExtraModel(new QStandardItemModel(this))
{
    connect(mExtraModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),this, SLOT(onExtraRowsInserted(const QModelIndex&, int, int)));
    connect(mExtraModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),this, SLOT(onExtraRowsRemoved(const QModelIndex&, int, int)));
    connect(mExtraModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(onExtraDataChanged(const QModelIndex&, const QModelIndex&)));
}

QHash<int, QByteArray> ProxyModel::roleNames () const
{
    return mModel == nullptr ? QHash<int, QByteArray>() : mModel->roleNames();
}

void ProxyModel::setSourceModel (QAbstractListModel *sourceModel)
{
    if (mModel) {
        disconnect(mModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),this, SLOT(onSourceRowsRemoved(const QModelIndex&, int, int)));
        disconnect(mModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),this, SLOT(onSourceRowsInserted(const QModelIndex&, int, int)));
        disconnect(mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),this, SLOT(onSourceDataChanged(const QModelIndex&, const QModelIndex&)));
    }

    mModel = sourceModel;

    beginResetModel();
    endResetModel();

    connect(mModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),this, SLOT(onSourceRowsRemoved(const QModelIndex&, int, int)));
    connect(mModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),this, SLOT(onSourceRowsInserted(const QModelIndex&, int, int)));
    connect(mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),this, SLOT(onSourceDataChanged(const QModelIndex&, const QModelIndex&)));
}

int ProxyModel::rowCount (const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return sourceRowCount() + mExtraModel->rowCount();
}

QVariant ProxyModel::data (const QModelIndex &index, int role) const
{
    if (index.row () < sourceRowCount ()) {
        return mModel->index (index.row (), 0).data (role);
    }

    return mExtraModel->index (index.row () - sourceRowCount (), 0).data (role);
}

QStandardItemModel *ProxyModel::extraRowModel ()
{
    return mExtraModel;
}

void ProxyModel::onSourceRowsRemoved (const QModelIndex &parent, int start, int end)
{
    Q_UNUSED (parent);

    beginRemoveRows (parent, start, end);

    endRemoveRows ();
}

void ProxyModel::onSourceRowsInserted (const QModelIndex &parent, int start, int end)
{
    Q_UNUSED (parent);

    beginInsertRows (parent, start, end);

    endInsertRows ();
}

void ProxyModel::onSourceDataChanged (const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    dataChanged (createIndex(topLeft.row(), 0), createIndex(bottomRight.row(), 0));
}

void ProxyModel::onExtraRowsRemoved (const QModelIndex &parent, int start, int end)
{
    Q_UNUSED (parent);

    beginRemoveRows (parent, sourceRowCount() + start, sourceRowCount() + end);

    endRemoveRows ();
}

void ProxyModel::onExtraRowsInserted (const QModelIndex &parent, int start, int end)
{
    Q_UNUSED (parent);

    beginInsertRows (parent, sourceRowCount() + start, sourceRowCount() + end);

    endInsertRows ();
}

void ProxyModel::onExtraDataChanged (const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    dataChanged (createIndex(sourceRowCount() + topLeft.row(), 0), createIndex(sourceRowCount() + bottomRight.row(), 0));
}

int ProxyModel::sourceRowCount () const
{
    return mModel == nullptr ? 0 : mModel->rowCount();
}
