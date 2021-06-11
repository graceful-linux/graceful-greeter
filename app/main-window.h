#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include <QLightDM/Greeter>

#include "login-form.h"

namespace graceful {
namespace Ui {
    class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(int screen, QWidget* parent = 0);
    ~MainWindow();

    void setFocus(Qt::FocusReason reason);

    bool showLoginForm();

    LoginForm* loginForm();

private:
    int getOffset(QString offset, int maxVal, int defaultVal);
    void setBackground();

private:
    int mScreen;
    LoginForm* mLoginForm;
};
}

#endif // MAINWINDOW_H
