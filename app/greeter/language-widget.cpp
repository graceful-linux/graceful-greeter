//
// Created by dingjing on 4/5/22.
//

#include "language-widget.h"

#include <QLabel>
#include <QEvent>
#include <QDebug>
#include <QAction>
#include <QListWidget>

LanguageWidget::LanguageWidget (QWidget *parent) : QMenu (parent), mLanguagesVector(getLanguages())
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    this->installEventFilter(this);
    initUserMenu();
}

void LanguageWidget::setCurrentLanguage (const QString &language)
{
    for (auto action : this->actions()) {
        if (action->data().toString() == language)
            this->setActiveAction(action);
    }
}


void LanguageWidget::initUserMenu ()
{
    connect(this, &QMenu::triggered, this, &LanguageWidget::onLanguageMenuTrigged);

    for (Language &lang : mLanguagesVector) {
        QString text = lang.name + (lang.territory.isEmpty() ? "" : ("-" + lang.territory));
        auto action = new QAction (this);
        action->setText(text);
        action->setData(lang.code);
        addAction(action);
    }
}

void LanguageWidget::onLanguageAdded(QString lang)
{
    Q_UNUSED(lang)
}

void LanguageWidget::onLanguageDeleted(QString lang)
{
    for (auto action : this->actions()) {
        if (action->data().toString() == lang)
            this->removeAction(action);
    }
}

void LanguageWidget::onLanguageMenuTrigged(QAction* action)
{
    QString languageCode = action->data().toString();
    auto iter = std::find_if (mLanguagesVector.begin(), mLanguagesVector.end(), [&] (const Language &language) { return language.code == languageCode;});

    Q_EMIT languageChanged(*iter);

    close();
}

bool LanguageWidget::eventFilter(QObject *obj, QEvent *event)
{
    //失去焦点时隐藏窗口
    if (event->type() == 23) {
        auto par = (QWidget*)this->parent();
        par->update();
        close();
    }

    return false;
}
