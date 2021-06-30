#include "login-form.h"

#include <QFile>
#include <QMenu>
#include <QDebug>
#include <QPixmap>
#include <QProcess>
#include <QCompleter>
#include <QModelIndex>
#include <QTextStream>
#include <QStringList>
#include <QMessageBox>
#include <QMetaMethod>
#include <QAbstractListModel>

#include <QLightDM/UsersModel>

#include "settings.h"
#include "ui_login-form.h"

const int KeyRole = QLightDM::SessionsModel::KeyRole;

using namespace graceful;

int rows(QAbstractItemModel& model)
{
    return model.rowCount(QModelIndex());
}

QString displayData(QAbstractItemModel& model, int row, int role)
{
    QModelIndex modelIndex = model.index(row, 0);

    return model.data(modelIndex, role).toString();
}

LoginForm::LoginForm(QWidget *parent) : QWidget(parent), ui(new Ui::LoginForm), mGreeter(), power(this), sessionsModel()
{
    if (!mGreeter.connectSync()) {
        close();
    }

    setGraphicsEffect(new QGraphicsOpacityEffect);

    ui->setupUi(this);
    initialize();
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::setFocus(Qt::FocusReason reason)
{
    if (ui->userInput->text().isEmpty()) {
        ui->userInput->setFocus(reason);
    } else {
        ui->passwordInput->setFocus(reason);
    }
}


void LoginForm::initialize()
{
    QPixmap icon(":/resources/rqt-2.png"); // This project came from Razor-qt
    ui->iconLabel->setPixmap(icon.scaled(ui->iconLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->hostnameLabel->setText(mGreeter.hostname());

    ui->sessionCombo->setModel(&sessionsModel);

    addLeaveEntry(power.canShutdown(), "system-shutdown", tr("Shutdown"), "shutdown");
    addLeaveEntry(power.canRestart(), "system-reboot", tr("Restart"), "restart");
    addLeaveEntry(power.canHibernate(), "system-suspend-hibernate", tr("Hibernate"), "hibernate");
    addLeaveEntry(power.canSuspend(), "system-suspend", tr("Suspend"), "suspend");
    ui->leaveComboBox->setDisabled(ui->leaveComboBox->count() <= 1);

    ui->sessionCombo->setCurrentIndex(0);
    setCurrentSession(mGreeter.defaultSessionHint());

    connect(ui->userInput, SIGNAL(editingFinished()), this, SLOT(userChanged()));
    connect(ui->leaveComboBox, SIGNAL(activated(int)), this, SLOT(leaveDropDownActivated(int)));
    connect(&mGreeter, SIGNAL(showPrompt(QString, QLightDM::Greeter::PromptType)), this, SLOT(onPrompt(QString, QLightDM::Greeter::PromptType)));
    connect(&mGreeter, SIGNAL(authenticationComplete()), this, SLOT(authenticationComplete()));

    ui->passwordInput->setEnabled(false);
    ui->passwordInput->clear();

    if (! mGreeter.hideUsersHint()) {
        QStringList knownUsers;
        QLightDM::UsersModel usersModel;
        for (int i = 0; i < usersModel.rowCount(QModelIndex()); i++) {
            knownUsers << usersModel.data(usersModel.index(i, 0), QLightDM::UsersModel::NameRole).toString();
        }
        ui->userInput->setCompleter(new QCompleter(knownUsers));
        ui->userInput->completer()->setCompletionMode(QCompleter::InlineCompletion);
    }

    QString user = Cache().getLastUser();
    if (user.isEmpty()) {
        user = mGreeter.selectUserHint();
    }
    ui->userInput->setText(user);
    userChanged();
}

void LoginForm::userChanged()
{
    setCurrentSession(Cache().getLastSession(ui->userInput->text()));

    if (mGreeter.inAuthentication()) {
        mGreeter.cancelAuthentication();
    }

    if (!ui->userInput->text().isEmpty()) {
        mGreeter.authenticate(ui->userInput->text());
        ui->passwordInput->setFocus();
    } else {
        ui->userInput->setFocus();
    }
}

void LoginForm::leaveDropDownActivated(int index)
{
    QString actionName = ui->leaveComboBox->itemData(index).toString();
    if      (actionName == "shutdown") power.shutdown();
    else if (actionName == "restart") power.restart();
    else if (actionName == "hibernate") power.hibernate();
    else if (actionName == "suspend") power.suspend();
}

void LoginForm::respond()
{
    mGreeter.respond(ui->passwordInput->text().trimmed());
    ui->passwordInput->clear();
    ui->passwordInput->setEnabled(false);
}

void LoginForm::onPrompt(QString prompt, QLightDM::Greeter::PromptType promptType)
{
    ui->passwordInput->setEnabled(true);
    ui->passwordInput->setFocus();
}


void LoginForm::addLeaveEntry(bool canDo, QString iconName, QString text, QString actionName)
{
    if (canDo) {
        ui->leaveComboBox->addItem(QIcon::fromTheme(iconName), text, actionName);
    }
}

QString LoginForm::currentSession()
{
    QModelIndex index = sessionsModel.index(ui->sessionCombo->currentIndex(), 0, QModelIndex());
    return sessionsModel.data(index, QLightDM::SessionsModel::KeyRole).toString();
}

void LoginForm::setCurrentSession(QString session)
{
    for (int i = 0; i < ui->sessionCombo->count(); i++) {
        if (session == sessionsModel.data(sessionsModel.index(i, 0), KeyRole).toString()) {
            ui->sessionCombo->setCurrentIndex(i);
            return;
        }
    }
}


void LoginForm::authenticationComplete()
{
    if (mGreeter.isAuthenticated()) {
        Cache().setLastUser(ui->userInput->text());
        Cache().setLastSession(ui->userInput->text(), currentSession());
        Cache().sync();
        mGreeter.startSessionSync(currentSession());
    } else  {
        ui->passwordInput->clear();
        userChanged();
    }
}

void LoginForm::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        respond();
    } else {
        QWidget::keyPressEvent(event);
    }
}

