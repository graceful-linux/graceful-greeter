//
// Created by dingjing on 4/2/22.
//

#ifndef GRACEFUL_GREETER_GLOBAL_H
#define GRACEFUL_GREETER_GLOBAL_H

#include <QFile>
#include <QPixmap>
#include <QString>
#include <QLocale>
#include <QSettings>

#define STR(s)          #s
#define GET_STR(s)      STR(s)

#define PREFIX          QString(GET_STR(RESOURCE_PATH))
#define IMAGE_DIR       PREFIX+"/images/"
#define QM_DIR          PREFIX+"/translations/"

class QRect;
class QPoint;

extern float            gScale;         // 缩放比例
extern int              gFontSize;      // 字体大小

bool isPicture (const QString& filepath);
QString ElideText (const QFont& font,int width,QString strInfo);
QPixmap scaledPixmap (int width, int height, const QString& url);

#endif //GRACEFUL_GREETER_GLOBAL_H
