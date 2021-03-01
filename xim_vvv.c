#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <assert.h>

Display *dpy;
Window win;
int scr;

void send_spot(XIC ic, XPoint nspot)
{
    XVaNestedList preedit_attr;
    preedit_attr = XVaCreateNestedList(0, XNSpotLocation, &nspot, NULL);
    XSetICValues(ic, XNPreeditAttributes, preedit_attr, NULL);
    XFree(preedit_attr);
}

static int preedit_start_callback(
    XIM xim,
    XPointer client_data,
    XPointer call_data)
{
    printf("preedit start\n");
    // no length limit
    return -1;
}

static void preedit_done_callback(
    XIM xim,
    XPointer client_data,
    XPointer call_data)
{
    printf("preedit done\n");
}

static void preedit_draw_callback(
    XIM xim,
    XPointer client_data,
    XIMPreeditDrawCallbackStruct *call_data)
{

    printf("callback\n");
    XIMText *xim_text = call_data->text;
    if (xim_text != NULL)
    {
        printf("Draw callback string: %s, length: %d, first: %d, caret: %d\n", xim_text->string.multi_byte, call_data->chg_length, call_data->chg_first, call_data->caret);
    }
    else
    {
        printf("Draw callback string: (DELETED), length: %d, first: %d, caret: %d\n", call_data->chg_length, call_data->chg_first, call_data->caret);
    }
}

static void preedit_caret_callback(
    XIM xim,
    XPointer client_data,
    XIMPreeditCaretCallbackStruct *call_data)
{
    printf("preedit caret\n");
    if (call_data != NULL)
    {
        printf("direction: %d position: %d\n", call_data->direction, call_data->position);
    }
}

int main()
{
    setlocale(LC_CTYPE, "");
    XSetLocaleModifiers("");

    dpy = XOpenDisplay(NULL);
    scr = DefaultScreen(dpy);
    win = XCreateSimpleWindow(dpy,
                              XDefaultRootWindow(dpy),
                              0, 0, 600, 400, 50,
                              BlackPixel(dpy, scr),
                              BlackPixel(dpy, scr));
    XMapWindow(dpy, win);
    XIMCallback draw_callback;
    draw_callback.client_data = NULL;
    draw_callback.callback = (XIMProc)preedit_draw_callback;
    //XIMCallback start_callback;
    //start_callback.client_data = NULL;
    //start_callback.callback = (XIMProc)preedit_start_callback;
    //XIMCallback done_callback;
    //done_callback.client_data = NULL;
    //done_callback.callback = (XIMProc)preedit_done_callback;
    //XIMCallback caret_callback;
    //caret_callback.client_data = NULL;
    //caret_callback.callback = (XIMProc)preedit_caret_callback;
    //XVaNestedList preedit_attributes = XVaCreateNestedList(
    //    0,
    //    XNPreeditStartCallback, &start_callback,
    //    XNPreeditDoneCallback, &done_callback,
    //    XNPreeditDrawCallback, &draw_callback,
    //    XNPreeditCaretCallback, &caret_callback,
    //    NULL);

    XVaNestedList preedit_attributes = XVaCreateNestedList(
        0,
        XNPreeditDrawCallback, &draw_callback,
        NULL);

    XIM xim = XOpenIM(dpy, NULL, NULL, NULL);
    XIC ic = XCreateIC(xim,
                       XNInputStyle, XIMPreeditCallbacks | XIMStatusNothing,
                       XNClientWindow, win,
                       XNPreeditAttributes, preedit_attributes,
                       NULL);
    XSetICFocus(ic);
    XSelectInput(dpy, win, KeyPressMask);
    XPoint spot;
    spot.x = 0;
    spot.y = 0;
    send_spot(ic, spot);

    static char *buff;
    size_t buff_size = 16;
    buff = (char *)malloc(buff_size);
    for (;;) {
        KeySym ksym;
        Status status;
        XEvent ev;
        XNextEvent(dpy, &ev);
        if (XFilterEvent(&ev, None)) {
            printf("XFilterEvent true\n");
            continue;
        }
        if (ev.type == KeyPress) {
            size_t c = Xutf8LookupString(ic, &ev.xkey,
                                         buff, buff_size - 1,
                                         &ksym, &status);
            if (status == XBufferOverflow) {
                printf("reallocate to the size of: %lu\n", c + 1);
                buff = realloc(buff, c + 1);
                c = XmbLookupString(ic, &ev.xkey,
                                    buff, c,
                                    &ksym, &status);
            }
            if (c) {
                spot.x += 20;
                //spot.y += 20;
                send_spot(ic, spot);
                buff[c] = 0;
                printf("delievered string: %s\n", buff);
            }
        }
    }
}
