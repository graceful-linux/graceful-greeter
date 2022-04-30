//
// Created by dingjing on 4/2/22.
//

#ifndef GRACEFUL_GREETER_PROXY_MODEL_H
#define GRACEFUL_GREETER_PROXY_MODEL_H

#include <QWeakPointer>
#include <QAbstractListModel>
#include <QStandardItemModel>

class ProxyModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ProxyModel (QObject *parent = 0);
    QHash<int, QByteArray> roleNames () const override;
    QVariant data (const QModelIndex &index, int role) const override;
    int rowCount (const QModelIndex &parent = QModelIndex()) const override;

    QStandardItemModel* extraRowModel ();
    void setSourceModel (QAbstractListModel *sourceModel);

public Q_SLOTS:
    void onSourceRowsRemoved (const QModelIndex & parent, int start, int end);
    void onSourceRowsInserted (const QModelIndex & parent, int start, int end);
    void onSourceDataChanged (const QModelIndex & topLeft, const QModelIndex & bottomRight);

    void onExtraRowsRemoved (const QModelIndex & parent, int start, int end);
    void onExtraRowsInserted (const QModelIndex & parent, int start, int end);
    void onExtraDataChanged (const QModelIndex & topLeft, const QModelIndex & bottomRight);

private:
    int sourceRowCount () const;

private:
    QAbstractListModel             *mModel = nullptr;
    QStandardItemModel             *mExtraModel = nullptr;

};

#endif //GRACEFUL_GREETER_PROXY_MODEL_H
