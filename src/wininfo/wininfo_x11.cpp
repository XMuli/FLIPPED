﻿/*******************************************************************
 * Copyright (c) 2021-2022 偕臧  All rights reserved.
 *
 * Author: XMuli <xmulitech@gmail.com>
 * GitHub: https://github.com/XMuli
 * Blog:   https://xmuli.tech
 *
 * Date: 2022.05.11
 * Description: Linux 系统下智能窗口识别
 ******************************************************************/
#include "wininfo_x11.h"
#include "../core/xlog/xlog.h"

#include <QLabel>


const QSize IMAGE_SIZE(400, 300);
const QSize ITEM_SIZE(400, 300);

void WinInfo_x11::setWinFilter(WinID target)
{
    m_vHWndFilter.push_back(target);
}

void WinInfo_x11::getWinInfoFromPoint(WinData &winData, QPoint pt, bool bPrevCache)
{
    test();
//    getWindowsList();
//    test();
//    POINT pos;
//    pos.x = pt.x;
//    pos.y = pt.y;

//    HWND hWndTarget = nullptr;

//    if (bPrevCache) {
//        getAllWinInfoCache();
//        hWndTarget = getWinInfoFromCache(pos);
//    } else {
//        hWndTarget = getAllWinInfoRealTime(pos);
//    }

    for (WinData& it : m_vWinList) {
        const QRect rt(it.rect);
        if (rt.contains(pt)) {
            winData = it;
            break;
        }
    }
}

WinInfo_x11::WinInfo_x11()
    : IWinInfo()
    , m_pDisplay(XOpenDisplay(nullptr))
{

}

WinInfo_x11::~WinInfo_x11()
{
    if (m_pDisplay)
        XCloseDisplay(m_pDisplay);

    m_pDisplay = nullptr;
    m_listWin.clear();
}

std::list<Window> WinInfo_x11::getWindowsList()
{
    getTopLevelWindows();
    return m_listWin;
}

static int g_index = 0;
void WinInfo_x11::test()
{
    m_listWin.clear();
    m_vWinList.clear();
    // 将窗口显示到QListWidget中
    int index = 0;


//    if (!m_listWidget) {
//        m_listWidget= new QListWidget();
//        m_listWidget->setIconSize(QSize(400, 300));
//    }



    for (Window win : getWindowsList()) {    //获取窗口属性
        XWindowAttributes attrs;
        XGetWindowAttributes(m_pDisplay, win, &attrs);    //XGetImage获取XImage，并通过转换得到QPixmap

//        XTextProperty text;
//        XGetWMName(m_pDisplay, win, &text);



//        m_hWndTarget = hWnd;
        WinID winId;
        winId._xWindow = win;
        m_vWinList.push_back(WinData(QRect(attrs.x, attrs.y, attrs.width, attrs.height)
                                          , winId
                                          , ""
                                          , ""
                                          , QString(getWindowAtom(win, "_NET_WM_NAME").c_str())
                                          , ""
                                          , 0
                                          , g_index++
                                          , 0));

    }

    for (Window win : getWindowsList()) {    //获取窗口属性
        XWindowAttributes attrs;
        XGetWindowAttributes(m_pDisplay, win, &attrs);    //XGetImage获取XImage，并通过转换得到QPixmap
        XImage * pImage = XGetImage(m_pDisplay, win, 0, 0, attrs.width, attrs.height, AllPlanes, ZPixmap);
        QImage image = QImage((const uchar *)(pImage->data), pImage->width, pImage->height, pImage->bytes_per_line, QImage::Format_RGB32);
        QPixmap pixmap = QPixmap::fromImage(image);
       //将QPixmap和窗口名添加到QListWidgetItem中，并将item添加到QListWidget中
        QListWidgetItem *listWidgetItemScreen = new QListWidgetItem(QIcon(pixmap.scaled(IMAGE_SIZE)), getWindowName(win).c_str());
        listWidgetItemScreen->setSizeHint(ITEM_SIZE);
//        m_listWidget->insertItem(index++, listWidgetItemScreen);

        WinID winId;
        winId._xWindow = win;
        m_vWinList.push_back(WinData(QRect(attrs.x, attrs.y, attrs.width, attrs.height)
                                          , winId
                                          , ""
                                          , ""
                                          , QString(getWindowAtom(win, "_NET_WM_NAME").c_str())
                                          , ""
                                          , 0
                                          , g_index++
                                          , 0));
    }
    // 获取整个屏幕图像并显示到QListWidget中
    //获取屏幕Window属性使用RootWindow函数获取
    Window rootwin = RootWindow(m_pDisplay,0);
    XWindowAttributes attrs;
    XGetWindowAttributes(m_pDisplay, rootwin, &attrs);
    XImage * pImage = XGetImage(m_pDisplay, rootwin, 0, 0, attrs.width, attrs.height, AllPlanes, ZPixmap);
    QImage image = QImage((const uchar *)(pImage->data), pImage->width, pImage->height, pImage->bytes_per_line, QImage::Format_RGB32);
    QPixmap pixmap = QPixmap::fromImage(image);
    QListWidgetItem *listWidgetItemScreen = new QListWidgetItem(QIcon(pixmap.scaled(IMAGE_SIZE)), "Screen");
    listWidgetItemScreen->setSizeHint(ITEM_SIZE);
//    m_listWidget->insertItem(index++, listWidgetItemScreen);

//    if (m_listWidget)
//        m_listWidget->show();
}

bool WinInfo_x11::bSupportQuery()
{
    Atom netSupportingWmCheck = XInternAtom(m_pDisplay, "_NET_SUPPORTING_WM_CHECK", true);

    Atom actualType = 0;
    int format = 0;
    unsigned long num = 0;
    unsigned long bytes = 0;
    unsigned char *data = nullptr;
    int status = XGetWindowProperty(m_pDisplay, DefaultRootWindow(m_pDisplay), netSupportingWmCheck, 0L, 1L, false
                                    , XA_WINDOW, &actualType, &format, &num, &bytes, &data);

    Window hWnd = 0;
    if (status == Success) {
        if (num > 0)
            hWnd = ((Window *)data)[0];

        if (data) {
            XFree(data);
            data = nullptr;
        }
    }

    if (hWnd) {
        status = XGetWindowProperty(m_pDisplay, hWnd, netSupportingWmCheck, 0L, 1L, false
                                    , XA_WINDOW, &actualType, &format, &num, &bytes, &data);

        if (status != Success || num == 0 || hWnd != ((Window *)data)[0])
            hWnd = 0;

        if (status == Success && data)
            XFree(data);
    }

    return hWnd != 0;
}

void WinInfo_x11::getTopLevelWindows()
{
    if (!bSupportQuery()) {
        XLOG_ERROR("Unable to query window list because window manager does not support extended window manager Hints");
        return;
    }

    Atom actualType = 0;
    int format = 0;
    unsigned long num = 0;
    unsigned long bytes = 0;
    Window *data = 0;
    Atom netClList = XInternAtom(m_pDisplay, "_NET_CLIENT_LIST", true);

    for (int i = 0; i < ScreenCount(m_pDisplay); ++i) {
        Window rootWin = RootWindow(m_pDisplay, i);

        int status = XGetWindowProperty(m_pDisplay, rootWin, netClList, 0L, ~0L, false
                                        , AnyPropertyType, &actualType, &format, &num, &bytes, (uint8_t **)&data);

        if (status != Success) {
            XLOG_ERROR("Failed getting root window properties");
            continue;
        }

        for (unsigned long i = 0; i < num; ++i)
            m_listWin.push_back(data[i]);

        XFree(data);
    }
}

std::string WinInfo_x11::getWindowAtom(Window win, const char *atom)
{
    XTextProperty tp;
    Atom netWmName = XInternAtom(m_pDisplay, atom, false);
    XGetTextProperty(m_pDisplay, win, &tp, netWmName);

    if (!tp.nitems) {
        XGetWMName(m_pDisplay, win, &tp);
        return "error";
    }

    std::string m_listWin = "unknown";

    if (tp.encoding == XA_STRING) {
        m_listWin = (char *)tp.value;
    } else {
        int n = 0;
        char **list = 0;
        int ret = XmbTextPropertyToTextList(m_pDisplay, &tp, &list, &n);

        if (ret >= Success && n > 0 && *list) {
            m_listWin = *list;
            XFreeStringList(list);
        }
    }

    XFree(tp.value);

    return m_listWin;
}

std::string WinInfo_x11::getWindowName(Window win)
{
    return getWindowAtom(win, "_NET_WM_NAME");
}
