//
// Created by dingjing on 4/4/22.
//

#include "utils.h"

#include <QApplication>
#include <QDesktopWidget>

#include <QRect>
#include <QDebug>
#include <QScreen>
#include <QX11Info>

#include <Imlib2.h>
#include <X11/Xlib.h>

Pixmap                  pix;
Window                  root = 0;
Display                *dpy = nullptr;
Screen                 *scn = nullptr;

void Utils::drawBackground ()
{
    XSetWindowBackgroundPixmap (dpy, root, pix);
    XClearWindow (dpy, root);

    while (XPending (dpy)) {
        XEvent ev;
        XNextEvent (dpy, &ev);
    }

    XFreePixmap (dpy, pix);
    XCloseDisplay (dpy);
}

void Utils::setRootWindowBackground (bool type, unsigned int color, char *filename)
{
    Imlib_Image img;

    if (!dpy) {
        dpy = XOpenDisplay (nullptr);
        if(!dpy) {
            return;
        }
    }

    double          width = 0;
    double          height = 0;

    width = QApplication::desktop ()->geometry ().width () * qApp->devicePixelRatio();
    height = QApplication::desktop ()->geometry ().height () * qApp->devicePixelRatio();

    if (!scn) {
        scn = DefaultScreenOfDisplay (dpy);
    }

    if (!root) {
        root = DefaultRootWindow (dpy);
    }

    pix = XCreatePixmap (dpy, root, (unsigned int) width, (unsigned int) height, DefaultDepthOfScreen(scn));

    imlib_context_set_display (dpy);
    imlib_context_set_visual (DefaultVisualOfScreen(scn));
    imlib_context_set_colormap (DefaultColormapOfScreen(scn));
    imlib_context_set_drawable (pix);

    if (type == 0) {
        img = imlib_load_image (filename);
        if (!img) {
            qWarning() << "无法读取图片: " << filename;
//            return ;
            type = 1;
        } else {
            imlib_context_set_image (img);
        }
    }

    if (type == 1) {
        img = imlib_create_image ((int) width, (int) height);
        imlib_context_set_image (img);
        unsigned int blue = color & 0xFF;
        unsigned int green = color >> 8 & 0xFF;
        unsigned int red = color >> 16 & 0xFF;

        imlib_context_set_color ((int) red, (int) green, (int) blue, 255);
        imlib_image_fill_rectangle(0, 0, (int) width, (int) height);
    }

    imlib_context_set_image(img);

    for (QScreen *screen : QApplication::screens()) {
        // 在每个屏幕上绘制背景
        QRect rect = screen->geometry();
        imlib_render_image_on_drawable_at_size((int) (rect.x() * screen->devicePixelRatio()),
                                               (int) (rect.y()*screen->devicePixelRatio()),
                                               (int) (rect.width()*screen->devicePixelRatio()),
                                               (int) (rect.height()*screen->devicePixelRatio()));
    }

    imlib_free_image();
}
