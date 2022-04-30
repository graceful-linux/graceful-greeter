//
// Created by dingjing on 4/2/22.
//

#include "global.h"

#include <QFont>
#include <QProcess>
#include <QPainter>
#include <QMimeDatabase>
#include <QtSvg/QSvgRenderer>

bool isPicture (const QString& filepath)
{
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile (filepath);

    return mime.name().startsWith("image/");
}

QString ElideText (const QFont& font, int width, QString strInfo)
{
    QFontMetrics fontMetrics (font);

    if(fontMetrics.width(strInfo) > width) {
        strInfo= QFontMetrics(font).elidedText(strInfo, Qt::ElideRight, width);
    }

    return strInfo;
}

QPixmap scaledPixmap (int width, int height, const QString& url)
{
    QFile imgFile (url);
    if(!imgFile.exists()) {
        return QPixmap();
    }

    QImage image(url);
    QPixmap pixmap = QPixmap::fromImage(image);
    if (pixmap.isNull()) {
        QProcess exec;
        QString program("file " + url);
        exec.start(program);
        exec.waitForFinished(1000);
        QString output = exec.readAllStandardOutput();
        if (output.contains("SVG")) {
            QSvgRenderer render(url);
            QImage image(width, height, QImage::Format_ARGB32);
            image.fill(Qt::transparent);
            QPainter painter(&image);
            render.render(&painter, image.rect());
            pixmap.convertFromImage(image);
        }
    }

    return pixmap.scaled(width, height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
}
