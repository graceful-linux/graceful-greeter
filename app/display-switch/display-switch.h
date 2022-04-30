#ifndef DISPLAYSWITCH_H
#define DISPLAYSWITCH_H

#include <QMainWindow>

namespace Ui
{
class MainWindow;
}

class DisplaySwitch : public QMainWindow
{
    Q_OBJECT
public:
    explicit DisplaySwitch(QWidget *parent = nullptr);
    void initUi();

Q_SIGNALS:

public Q_SLOTS:
    void onDisplaySwitchSelect();
    void onDisplaySwitchConfirm();
    void onPositionChanged(const QRect&);
    void onGlobalKeyRelease(const QString &key);

private:
    Ui::MainWindow *ui;
    int     currentMode;
    bool    selecting;
};

#endif // DISPLAYSWITCH_H
