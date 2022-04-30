//
// Created by dingjing on 4/2/22.
//

#include "fake-dialog.h"

#include <QKeyEvent>
#include <QPushButton>

#define BORDER_WIDTH        24

FakeDialog::FakeDialog (QWidget *parent) : QWidget(parent)
{
    initUI();
}

QWidget *FakeDialog::centerWidget ()
{
    return mCenterWidget;
}

QWidget *FakeDialog::dialog ()
{
    return mDialog;
}

void FakeDialog::setDialogSize (int w, int h)
{
    mDialogWidth = w;
    mDialogHeight = h;

    resizeEvent (nullptr);
}

void FakeDialog::resizeEvent (QResizeEvent *event)
{
    mDialog->setGeometry((width() - mDialogWidth) / 2, (height() - mDialogHeight) / 2, mDialogWidth, mDialogHeight);
    mCloseButton->setGeometry(mCenterWidget->width()*2/3, mCenterWidget->y() + mCenterWidget->height() - 80,80, 30);
    mCenterWidget->setGeometry(BORDER_WIDTH, BORDER_WIDTH,mDialog->width() - BORDER_WIDTH * 2 ,mDialog->height() - BORDER_WIDTH * 2);
}

void FakeDialog::keyReleaseEvent (QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void FakeDialog::initUI ()
{
    setGeometry(parentWidget()->rect());
    setObjectName("fakeDialogBackground");
    setAttribute(Qt::WA_TranslucentBackground, true);

    mDialog = new QWidget(this);
    mDialog->setObjectName(QStringLiteral("fakeDialog"));

    mCenterWidget = new QWidget(mDialog);
    mCenterWidget->setObjectName(QStringLiteral("dialogCenterWidget"));

    mCloseButton = new QPushButton(mDialog);
    mCloseButton->setText(tr("cancel"));

    connect (mCloseButton, &QPushButton::clicked, this, &FakeDialog::close);
}


