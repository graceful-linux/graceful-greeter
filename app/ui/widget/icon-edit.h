//
// Created by dingjing on 4/5/22.
//

#ifndef GRACEFUL_GREETER_ICON_EDIT_H
#define GRACEFUL_GREETER_ICON_EDIT_H
#include <QLabel>
#include <QPixmap>
#include <QLineEdit>
#include <QSettings>
#include <QPushButton>

class Configuration;
class IconEdit : public QWidget
{
    Q_OBJECT
public:
    explicit IconEdit(QWidget *parent = nullptr);

    void setIcon(const QIcon &icon);
    void setIcon(const QString &text);

    void stopWaiting();
    void startWaiting();

    void clear();
    void setCapsState(bool capsState);

    void setType(QLineEdit::EchoMode type = QLineEdit::Password);

protected:
    void resizeEvent(QResizeEvent *) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void updatePixmap();

Q_SIGNALS:
    void authenticationFinished();
    void startAuthentication(const QString&);

private Q_SLOTS:
    void onGlobalKeyRelease(const QString &key);

private:
    QLineEdit                      *mEdit;
    QLabel                         *mCapsIcon;
    QPushButton                    *mIconButton;
    QPushButton                    *mModeButton;
    QTimer                         *mTimer;
    QPixmap                         mWaitingPixmap;
    QString                         mIconText;
    QIcon                           mIcon;
};


#endif //GRACEFUL_GREETER_ICON_EDIT_H
