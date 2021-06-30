#include <QRect>
#include <QDebug>
#include <QString>
#include <QPalette>
#include <QApplication>
#include <QDesktopWidget>

#include "settings.h"
#include "login-form.h"
#include "main-window.h"

using namespace graceful;

MainWindow::MainWindow(int screen, QWidget *parent) : QWidget(parent), mScreen(screen)
{
    setObjectName(QString("MainWindow_%1").arg(screen));
    
    QRect screenRect = QApplication::desktop()->screenGeometry(screen);
    setGeometry(screenRect);

    setBackground();

    if (showLoginForm()) {
        mLoginForm = new LoginForm(this);

        int maxX = screenRect.width() - mLoginForm->width();
        int maxY = screenRect.height() - mLoginForm->height();
        int defaultX = 10 * maxX / 100;
        int defaultY = 50 * maxY / 100;
        int offsetX = getOffset(Settings().offsetX(), maxX, defaultX);
        int offsetY = getOffset(Settings().offsetY(), maxY, defaultY);
        
        mLoginForm->move(offsetX, offsetY);
        mLoginForm->show();

        int centerX = screenRect.width() / 2 + screenRect.x();
        int centerY = screenRect.height() / 2 + screenRect.y();
        QCursor::setPos(centerX, centerY);
    }
}

MainWindow::~MainWindow()
{

}

bool MainWindow::showLoginForm()
{
    return mScreen == QApplication::desktop()->primaryScreen();
}

LoginForm *MainWindow::loginForm()
{
    return mLoginForm;
}

void MainWindow::setFocus(Qt::FocusReason reason)
{
    if (mLoginForm) {
        mLoginForm->setFocus(reason);
    } else  {
        QWidget::setFocus(reason);
    }
}

int MainWindow::getOffset(QString settingsOffset, int maxVal, int defaultVal)
{
    int offset = defaultVal > maxVal ? maxVal : defaultVal;

    if (! settingsOffset.isEmpty())  {
        if (QRegExp("^\\d+px$", Qt::CaseInsensitive).exactMatch(settingsOffset))  {
            offset = settingsOffset.left(settingsOffset.size() - 2).toInt();
            if (offset > maxVal) {
                offset = maxVal;
            }
        } else if (QRegExp("^\\d+%$", Qt::CaseInsensitive).exactMatch(settingsOffset)) {
            int offsetPct = settingsOffset.left(settingsOffset.size() -1).toInt();
            if (offsetPct > 100) {
                offsetPct = 100;
            }
            offset = (maxVal * offsetPct) / 100;
        } else {
            qWarning() << "Could not understand" << settingsOffset
                       << "- must be of form <positivenumber>px or <positivenumber>%, e.g. 35px or 25%" ;
        }
    }

    return offset;
}

void MainWindow::setBackground()
{
    QPixmap backgroundImage;
    QSettings greeterSettings(CONFIG_FILE, QSettings::IniFormat);
    
    if (greeterSettings.contains(BACKGROUND_IMAGE_KEY)) {
        QString pathToBackgroundImage = greeterSettings.value(BACKGROUND_IMAGE_KEY).toString();
        
        backgroundImage = QPixmap(pathToBackgroundImage);
        if (backgroundImage.isNull()) {
            qWarning() << "Not able to read" << pathToBackgroundImage << "as image";
        }
    } else {
        backgroundImage = QPixmap(":/greeter/bg.jpg");
    }

//    backgroundImage.fill(Qt::transparent);
//    backgroundImage.setGraphicsEffect(new QGraphicsBlurEffect);
    
    QPalette palette;
    QRect rect = QApplication::desktop()->screenGeometry(mScreen);
    if (backgroundImage.isNull()) {
        palette.setColor(QPalette::Background, Qt::black);
    } else {
        QBrush brush(backgroundImage.scaled(rect.width(), rect.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

        palette.setBrush(this->backgroundRole(), brush);
    }

    this->setPalette(palette);
}


