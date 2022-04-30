//
// Created by dingjing on 4/2/22.
//

#include "border-dialog.h"

#include <QDebug>
#include <QPainter>
#include <QPalette>
#include <QMouseEvent>
#include <QPushButton>
#include <QPropertyAnimation>

BorderDialog::BorderDialog (QWidget *parent) : QWidget (parent)
{
    initUI();
}

void BorderDialog::close ()
{
    mBorderWidget->close();
    startBackground(80, 0, false);
}

void BorderDialog::setBorderWidth (int width)
{
    borderWidth = width;
    setDialogSize(this->width(), this->height());
}

void BorderDialog::setDialogSize (int w, int h)
{
    mBorderWidget->setGeometry((width() - w) / 2,(height() - h) / 2, w, h);
    mCenterWidget->setGeometry(borderWidth, borderWidth,mBorderWidget->width() - borderWidth * 2,mBorderWidget->height() - borderWidth * 2);

    mClose->setGeometry(QRect(0, 0, borderWidth, borderWidth));
}

void BorderDialog::setDialogColor (QColor color)
{
    setWidgetColor(mBorderWidget, color);
}

void BorderDialog::setBorderColor (QColor color)
{
    setWidgetColor(mBorderWidget, color);
}

void BorderDialog::initUI ()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setGeometry(parentWidget()->rect());

    mBorderWidget = new QWidget(this);
    mBorderWidget->setObjectName("BorderDialogBorder");
    mCenterWidget = new QWidget(mBorderWidget);
    mCenterWidget->setObjectName("BorderDialogWidget");

    /* 窗口关闭按钮 */
    mClose = new QPushButton(mBorderWidget);
    mClose->setObjectName(QStringLiteral("close"));
    connect(mClose, &QPushButton::clicked, this, &BorderDialog::close);

    setBorderWidth();
    setBorderColor();
    setDialogColor();
}

QWidget *BorderDialog::center () const
{
    return mCenterWidget;
}

int BorderDialog::opacity () const
{
    return mOpacity;
}

void BorderDialog::setOpacity (int opacity)
{
    mOpacity = opacity;
    update();
}

void BorderDialog::setWidgetColor (QWidget *widget, QColor color)
{
    QString borderStyle = QString("#%1{background: rgb(%2, %3, %4, %5)}")
            .arg(widget->objectName())
            .arg(QString::number(color.red()))
            .arg(QString::number(color.green()))
            .arg(QString::number(color.blue()))
            .arg(QString::number(color.alpha()));

    widget->setStyleSheet(borderStyle);
}

void BorderDialog::startBackground (int begin, int end, bool show)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "opacity");
    animation->setDuration(300);
    animation->setStartValue(begin);
    animation->setEndValue(end);
    if (!show) {
        connect(animation, &QPropertyAnimation::finished, this, &QWidget::close);
    }

    animation->start();
}

void BorderDialog::paintEvent (QPaintEvent* e)
{
    QPainter painter(this);

    painter.setPen(Qt::transparent);
    painter.setBrush(QColor(0, 0, 0, mOpacity));
    painter.drawRect(rect());

    QWidget::paintEvent(e);
}

void BorderDialog::showEvent (QShowEvent *event)
{
    startBackground(0, 80, true);
}

void BorderDialog::mousePressEvent (QMouseEvent *event)
{
    if(!mBorderWidget->geometry().contains(event->pos())) {
        close();
    }
}

