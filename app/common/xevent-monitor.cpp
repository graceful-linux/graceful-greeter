//
// Created by dingjing on 4/3/22.
//

#include "xevent-monitor.h"
#include <QSet>
#include <QVector>

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <X11/Xlibint.h>
#include <X11/extensions/record.h>


// Virtual button codes that are not defined by X11.
#define Button1                 1
#define Button2                 2
#define Button3                 3
#define WheelUp                 4
#define WheelDown               5
#define WheelLeft               6
#define WheelRight              7
#define XButton1                8
#define XButton2                9

QVector<KeySym> ModifiersVec {
        XK_Control_L,
        XK_Control_R,
        XK_Shift_L,
        XK_Shift_R,
        XK_Super_L,
        XK_Super_R,
        XK_Alt_L,
        XK_Alt_R
};

XEventMonitor *XEventMonitor::gInstance = nullptr;


class XEventMonitorPrivate
{
public:
    explicit XEventMonitorPrivate (XEventMonitor *parent);
    virtual ~XEventMonitorPrivate ();
    void run ();

protected:
    static bool filterWheelEvent (int detail);
    static void callback (XPointer ptr, XRecordInterceptData* data);
    void handleRecordEvent (XRecordInterceptData *);
    void emitButtonSignal (const char *member, xEvent *event);
    void emitKeySignal (const char *member, xEvent *event);
    void updateModifier (xEvent *event, bool isAdd);

protected:
    XEventMonitor              *q_ptr;
    QSet<KeySym>               mModifiers;

private:
    Q_DECLARE_PUBLIC (XEventMonitor)
};

bool checkCapsState ()
{
    bool capsState = false;
    unsigned int n;

    XkbGetIndicatorState (QX11Info::display(), XkbUseCoreKbd, &n);
    capsState = ((n & 0x01) == 1);

    return capsState;
}

bool checkNumLockState ()
{
    XKeyboardState x;
    XGetKeyboardControl(QX11Info::display(), &x);

    return x.led_mask & 2;
}

XEventMonitorPrivate::XEventMonitorPrivate(XEventMonitor *parent) : q_ptr(parent)
{

}

XEventMonitorPrivate::~XEventMonitorPrivate()
= default;

void XEventMonitorPrivate::emitButtonSignal(const char *member, xEvent *event)
{
    int x = event->u.keyButtonPointer.rootX;
    int y = event->u.keyButtonPointer.rootY;
    QMetaObject::invokeMethod (q_ptr, member,Qt::DirectConnection, Q_ARG(int, x), Q_ARG(int, y));
}

void XEventMonitorPrivate::emitKeySignal(const char *member, xEvent *event)
{
    Display *display = XOpenDisplay(nullptr);
    int keyCode = event->u.u.detail;
    KeySym keySym = XkbKeycodeToKeysym(display, event->u.u.detail, 0, 0);

    QString keyStrSplice;
    for (auto modifier : mModifiers) {
        keyStrSplice += QString(XKeysymToString(modifier)) + "+";
    }
    //按键是修饰键
    if (ModifiersVec.contains (keySym) && !mModifiers.isEmpty()) {
        keyStrSplice.remove (keyStrSplice.length () - 1, 1);
    } else {
        keyStrSplice += XKeysymToString (keySym);
    }

    QMetaObject::invokeMethod(q_ptr, member, Qt::AutoConnection, Q_ARG(int, keyCode));
    QMetaObject::invokeMethod(q_ptr, member, Qt::AutoConnection, Q_ARG(QString, keyStrSplice));

    XCloseDisplay(display);
}

void XEventMonitorPrivate::run ()
{
    Display* display = XOpenDisplay (nullptr);
    if (!display) {
        fprintf(stderr, "unable to open display\n");
        return;
    }

    // Receive from ALL clients, including future clients.
    XRecordClientSpec clients = XRecordAllClients;
    XRecordRange* range = XRecordAllocRange();
    if (!range) {
        fprintf(stderr, "unable to allocate XRecordRange\n");
        return;
    }

    // Receive KeyPress, KeyRelease, ButtonPress, ButtonRelease and MotionNotify events.
    memset(range, 0, sizeof(XRecordRange));
    range->device_events.first = KeyPress;
    range->device_events.last  = MotionNotify;

    // And create the XRECORD context.
    XRecordContext context = XRecordCreateContext(display, 0, &clients, 1, &range, 1);
    if (context == 0) {
        fprintf(stderr, "XRecordCreateContext failed\n");
        return;
    }
    XFree(range);

    XSync(display, True);

    Display* displayDataLink = XOpenDisplay(nullptr);
    if (!displayDataLink) {
        fprintf(stderr, "unable to open second display\n");
        return;
    }
    if (!XRecordEnableContext(displayDataLink, context,  callback, (XPointer) this)) {
        fprintf(stderr, "XRecordEnableContext() failed\n");
        return;
    }
}

void XEventMonitorPrivate::callback (const XPointer ptr, XRecordInterceptData* data)
{
    ((XEventMonitorPrivate *) ptr)->handleRecordEvent(data);
}

void XEventMonitorPrivate::handleRecordEvent(XRecordInterceptData* data)
{
    if (data->category == XRecordFromServer) {
        auto event = (xEvent *)data->data;
        switch (event->u.u.type) {
            case ButtonPress:
                if (filterWheelEvent(event->u.u.detail)) {
                    emitButtonSignal("buttonPress", event);
                }
                break;
            case MotionNotify:
                emitButtonSignal("buttonDrag", event);
                break;
            case ButtonRelease:
                if (filterWheelEvent(event->u.u.detail)) {
                    emitButtonSignal("buttonRelease", event);
                }
                break;
            case KeyPress:
                updateModifier(event, true);
                emitKeySignal("keyPress", event);
                break;
            case KeyRelease:
                updateModifier(event, false);
                emitKeySignal("keyRelease", event);
                break;
            default:
                break;
        }
    }

    fflush(stdout);
    XRecordFreeData(data);
}

bool XEventMonitorPrivate::filterWheelEvent (int detail)
{
    return detail != WheelUp && detail != WheelDown && detail != WheelLeft && detail != WheelRight;
}

void XEventMonitorPrivate::updateModifier(xEvent *event, bool isAdd)
{
    Display *display = XOpenDisplay(nullptr);
    KeySym keySym = XkbKeycodeToKeysym(display, event->u.u.detail, 0, 0);

    if(ModifiersVec.contains(keySym)) {
        if(isAdd) {
            mModifiers.insert(keySym);
        } else {
            mModifiers.remove(keySym);
        }
    }

    XCloseDisplay(display);
}

XEventMonitor *XEventMonitor::instance ()
{
    if (!gInstance) {
        gInstance = new XEventMonitor ();
    }

    return gInstance;
}

XEventMonitor::XEventMonitor (QObject *parent) : QThread (parent), d_ptr(new XEventMonitorPrivate(this))
{
    Q_D (XEventMonitor);
}

XEventMonitor::~XEventMonitor ()
{
    requestInterruption ();
    quit ();
    wait ();
}

void XEventMonitor::run ()
{
    if (!isInterruptionRequested()) {
        d_ptr->run();
    }
}
