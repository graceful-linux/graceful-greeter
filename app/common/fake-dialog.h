//
// Created by dingjing on 4/2/22.
//

#ifndef GRACEFUL_GREETER_FAKEDIALOG_H
#define GRACEFUL_GREETER_FAKEDIALOG_H
#include <QWidget>

class QPushButton;

class FakeDialog : public QWidget
{
    Q_OBJECT
public:
    explicit FakeDialog (QWidget *parent = nullptr);

    QWidget *dialog ();
    QWidget *centerWidget ();
    void setDialogSize (int w, int h);

protected:
    void resizeEvent (QResizeEvent *event) override;
    void keyReleaseEvent (QKeyEvent *event) override;

private:
    void initUI ();

private:
    int                 mDialogWidth;
    int                 mDialogHeight;

    QWidget            *mDialog;
    QWidget            *mCenterWidget;
    QPushButton        *mCloseButton;
};


#endif //GRACEFUL_GREETER_FAKEDIALOG_H
