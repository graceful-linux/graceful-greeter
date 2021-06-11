#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QMap>
#include <QWidget>
#include <QDialog>
#include <QProcess>
#include <QKeyEvent>
#include <QLightDM/Power>
#include <QLightDM/Greeter>
#include <QLightDM/SessionsModel>
#include <QGraphicsOpacityEffect>

namespace Ui
{
class LoginForm;
}

namespace graceful {

class LoginForm : public QWidget
{
    Q_OBJECT

friend class DecoratedUsersModel;

public:
    explicit LoginForm(QWidget *parent = 0);
    ~LoginForm();
    virtual void setFocus(Qt::FocusReason reason);

public slots:
    void userChanged();
    void leaveDropDownActivated(int index);
    void respond();
    void onPrompt(QString prompt, QLightDM::Greeter::PromptType promptType);
    void authenticationComplete();

protected:
    virtual void keyPressEvent(QKeyEvent *event);

private:
    void initialize();
    void addLeaveEntry(bool canDo, QString iconName, QString text, QString actionName);
    QString currentSession();
    void setCurrentSession(QString session);

    Ui::LoginForm *ui;

    QLightDM::Greeter mGreeter;
    QLightDM::PowerInterface power;
    QLightDM::SessionsModel sessionsModel;

    QMap<int, void (QLightDM::PowerInterface::*)()> powerSlots;
};

}
#endif // LOGINFORM_H
