//
// Created by dingjing on 4/2/22.
//

#ifndef GRACEFUL_GREETER_BORDERDIALOG_H
#define GRACEFUL_GREETER_BORDERDIALOG_H
#include <QWidget>

class QPushButton;

class BorderDialog : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int opacity READ opacity WRITE setOpacity)
public:
    explicit BorderDialog (QWidget *parent = 0);

    void close ();

    void setBorderWidth (int width = 24);
    void setDialogSize (int width, int h);
    void setDialogColor (QColor color = QColor("#2D698E"));
    void setBorderColor (QColor color = QColor(0, 0, 0, 50));

protected:
    void initUI ();
    QWidget* center () const;

private:
    int opacity () const;
    void setOpacity (int);
    void setWidgetColor (QWidget *widget, QColor color);
    void startBackground (int begin, int end, bool show);

protected:
    void paintEvent (QPaintEvent *) override;
    void showEvent (QShowEvent *event) override;
    void mousePressEvent (QMouseEvent *event) override;

Q_SIGNALS:
    void aboutToClose ();

private:
    qreal               mOpacity{};

    QWidget            *mCenterWidget{};
    QWidget            *mBorderWidget{};
    QPushButton        *mClose{};

    int                 borderWidth{};

};


#endif //GRACEFUL_GREETER_BORDERDIALOG_H
