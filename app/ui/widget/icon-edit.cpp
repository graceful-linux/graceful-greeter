//
// Created by dingjing on 4/5/22.
//

#include "icon-edit.h"

#include <QDebug>
#include <QTimer>
#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QHBoxLayout>

#include "../../common/xevent-monitor.h"

IconEdit::IconEdit(QWidget *parent) :
    QWidget(parent),
    mTimer(new QTimer (this))
{
    mEdit = new QLineEdit(this);
    mEdit->setObjectName(QStringLiteral("passwdEdit"));
    mEdit->setReadOnly (false);
    mEdit->setEnabled (true);

    mCapsIcon = new QLabel(this);
    mCapsIcon->setObjectName(QStringLiteral("capsIconLabel"));

    mIconButton = new QPushButton(this);
    mIconButton->setObjectName(QStringLiteral("loginButton"));
    mIconButton->setEnabled (true);

    mModeButton = new QPushButton(this);
    mModeButton->setObjectName(QStringLiteral("echoModeButton"));

    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins (1, 1, 1, 1);
    layout->setSpacing (0);
    layout->addStretch ();
    layout->addWidget (mCapsIcon);
    layout->addWidget (mModeButton);
    layout->addWidget (mIconButton);

    mEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    mEdit->setContextMenuPolicy(Qt::NoContextMenu);         
    mEdit->installEventFilter(this);
    mEdit->setMaxLength(1000);

    mIconButton->setFocusPolicy(Qt::NoFocus);
    mIconButton->setCursor(QCursor(Qt::PointingHandCursor));
    mIconButton->installEventFilter(this);

    mModeButton->setCheckable(true);
    mModeButton->setFocusPolicy(Qt::NoFocus);
    mModeButton->setCursor(Qt::PointingHandCursor);
    mEdit->setEchoMode (QLineEdit::Password);
    connect (mModeButton, &QPushButton::clicked, this, [&] (bool checked) {
        setType(checked ? QLineEdit::Normal : QLineEdit::Password);
    });

    mTimer->setInterval (600);
    connect (mTimer, &QTimer::timeout, this, &IconEdit::updatePixmap);
    connect (mEdit, &QLineEdit::returnPressed, mIconButton, &QPushButton::click);
    connect (mIconButton, &QPushButton::clicked, this, [=] () {
        startWaiting();
        Q_EMIT startAuthentication (mEdit->text());
    });
    connect (this, &IconEdit::authenticationFinished, this, [=] () {
        stopWaiting();
        mEdit->clear();
    });

    connect (XEventMonitor::instance(), SIGNAL(keyRelease(QString)), this, SLOT(onGlobalKeyRelease(QString)));

    setFocusProxy (mEdit);
    setCapsState (checkCapsState());
}

bool IconEdit::eventFilter (QObject *obj, QEvent *event)
{
    if (obj == mEdit) {
        if (event->type() == QEvent::KeyPress) {
            auto keyEvent = dynamic_cast<QKeyEvent *>(event);
            if (keyEvent->matches(QKeySequence::Copy)
                || keyEvent->matches(QKeySequence::Cut)
                || keyEvent->matches(QKeySequence::Paste)) {
                event->ignore();
                return true;
            } else if ((keyEvent->modifiers() & Qt::MetaModifier)
                || (keyEvent->modifiers() & Qt::AltModifier)) {
                event->ignore();
                return true;
            }
        }
    }

    if (obj == mIconButton) {
        if (mTimer && mTimer->isActive()) {
            return false;
        }
        if (event->type() == QEvent::HoverEnter) {
            setIcon(QIcon(":/images/images/login-button-hover.png"));
        } else if (event->type() == QEvent::HoverLeave) {
            setIcon(QIcon(":/images/images/login-button.png"));
        }
    }

    return false;
}

void IconEdit::setType(QLineEdit::EchoMode type)
{
    mEdit->setEchoMode (type);
    if(type == 0) {
        mModeButton->setChecked (true);
    } else {
        mModeButton->setChecked (false);
    }
}

void IconEdit::resizeEvent (QResizeEvent*)
{
    int w = mIconButton->width() + mModeButton->width();
    mEdit->setTextMargins(1, 1, mCapsIcon->isVisible() ? w + mCapsIcon->width() : w, 1);
    mEdit->setFixedSize(size());
}

void IconEdit::onGlobalKeyRelease(const QString &key)
{
    if (key == "Caps_Lock") {
        setCapsState(!mCapsIcon->isVisible());
    }
}

void IconEdit::setCapsState(bool capsState)
{
    mCapsIcon->setVisible(capsState);
    int w = mIconButton->width() + mModeButton->width();
    mEdit->setTextMargins(1, 1, capsState ? w + mCapsIcon->width() : w, 1);
}

void IconEdit::setIcon(const QString &text)
{
    mIconButton->setIcon(QIcon());
    mIconButton->setText(text);
    mIconText = text;
    mIcon = QIcon();
}

void IconEdit::setIcon(const QIcon &icon)
{
    mIconButton->setIcon(icon);
    mIconButton->setText("");
    mIcon = icon;
    mIconText = "";
}

void IconEdit::startWaiting ()
{
    mEdit->setReadOnly (true);
    mIconButton->setEnabled (false);

    mWaitingPixmap.load (":/images/images/waiting.png");
    mIconButton->setIconSize (mIconButton->size());
    mIconButton->setIcon (QIcon(mWaitingPixmap));
    mTimer->start ();
}

void IconEdit::stopWaiting ()
{
    if (mTimer->isActive ()) {
        mTimer->stop ();
    }

    mEdit->setReadOnly (false);
    mIconButton->setEnabled (true);
    if (!mIcon.isNull ()) {
        mIconButton->setIcon (mIcon);
    } else {
        mIconButton->setText (mIconText);
    }
}

void IconEdit::updatePixmap()
{
#if QT_DEPRECATED_SINCE(5, 15)
    QTransform trans;
    trans.translate(0, 50);
    trans.scale(1, -1);
    trans.translate(-50,-50);
    mWaitingPixmap = mWaitingPixmap.transformed(trans, Qt::FastTransformation);
#else
    QMatrix matrix;
    matrix.rotate(90.0);
    mWaitingPixmap = mWaitingPixmap.transformed(matrix, Qt::FastTransformation);
#endif
    mIconButton->setIcon(QIcon(mWaitingPixmap));
}

void IconEdit::clear ()
{
    mEdit->clear();
}