//
// Created by dingjing on 4/5/22.
//

#ifndef GRACEFUL_GREETER_LANGUAGEWIDGET_H
#define GRACEFUL_GREETER_LANGUAGEWIDGET_H
#include <QMenu>
#include <QWidget>

#include "language.h"
#include "../common/fake-dialog.h"

class QListWidget;
class QListWidgetItem;
class QLabel;
class LanguageWidget : public QMenu
{
    Q_OBJECT
public:
    explicit LanguageWidget(QWidget *parent = nullptr);
    void setCurrentLanguage(const QString &language);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void initUserMenu();

private Q_SLOTS:
    void onLanguageAdded(QString lang);
    void onLanguageDeleted(QString lang);
    void onLanguageMenuTrigged(QAction *action);

Q_SIGNALS:
    void languageChanged(const Language &language);

private:
    LanguagesVector            &mLanguagesVector;
    QMenu                      *mMenu;
};

#endif //GRACEFUL_GREETER_LANGUAGEWIDGET_H
