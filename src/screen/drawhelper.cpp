﻿/*******************************************************************
 * Copyright (c) 2021-2022 偕臧  All rights reserved.
 *
 * Author: XMuli <xmulitech@gmail.com>
 * GitHub: https://github.com/XMuli
 * Blog:   https://xmuli.tech
 *
 * Date:   2021.11.09
 * Description: 
 ******************************************************************/
#include "drawhelper.h"

#include <QScreen>
#include <QFile>
#include <QByteArray>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QSvgRenderer>
#include <QDomElement>
#include <QDomElement>
#include <QIcon>
#include <QGraphicsEffect>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QLine>


int XDrawStep::g_index = 0;

ScrnHelper::ScrnHelper(QObject *parent)
    : QObject(parent)
{
}

ScrnHelper::~ScrnHelper()
{
}


// 辅助功能函数
namespace XHelp {

/*!
 * \brief changeSVGColor 修改 .svg 的填充色
 * \param path   .svg 文件路径
 * \param shape  欲修改的图形， 通常是 path、偶尔是 rect 等
 * \param color  填充的颜色
 * \param size   svg 伸缩后的尺寸
 * \return 一个 QIcon 对象
 */
QIcon changeSVGColor(QString path, QString shape, QColor color, QSize size)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QByteArray baData = file.readAll();

    QDomDocument doc;
    doc.setContent(baData);
    QDomElement elem = doc.documentElement(); // const 和 值传递
    setAttrRecur(elem, shape, "fill", color.name());

    QSvgRenderer svgRenderer(doc.toByteArray());
    // create pixmap target (could be a QImage)
    QPixmap pix(svgRenderer.defaultSize().scaled(size.width(), size.height(), Qt::KeepAspectRatio));  // fix: 修改 svg 颜色后略有模糊
    pix.fill(Qt::transparent);
    // create painter to act over pixmap
    QPainter pixPainter(&pix);
    // use renderer to render over painter which paints on pixmap
    svgRenderer.render(&pixPainter);
    QIcon myicon(pix);
    return myicon;
}

void setAttrRecur(QDomElement &elem, QString strtagname, QString strattr, QString strattrval)
{
    // if it has the tagname then overwritte desired attribute
    if (elem.tagName().compare(strtagname) == 0)
        elem.setAttribute(strattr, strattrval);

    // loop all children
    for (int i = 0; i < elem.childNodes().count(); i++) {
        if (!elem.childNodes().at(i).isElement())
            continue;

        QDomElement t = elem.childNodes().at(i).toElement();
        setAttrRecur(t, strtagname, strattr, strattrval);
    }
}

const QPixmap* SetMosaicSmooth(QPixmap* pixmap, int px)
{
    if (!pixmap)
        return nullptr;

    QGraphicsBlurEffect* blur = new QGraphicsBlurEffect;
    blur->setBlurRadius(10);
    QGraphicsPixmapItem* item =
        new QGraphicsPixmapItem(*pixmap);
    item->setGraphicsEffect(blur);

    QGraphicsScene scene;
    scene.addItem(item);

    QPainter painter(pixmap);
    scene.render(&painter, pixmap->rect(), QRectF());
    blur->setBlurRadius(12);
    // multiple repeat for make blur effect stronger
    scene.render(&painter, pixmap->rect(), QRectF());

    //pixmap->save("hahhaha.png");

    // TODO 2022-01-09:  QGraphicsBlurEffect
    // key: qt 截图 毛玻璃效果
    return pixmap;
}

const QImage SetMosaicPixlelated(QPixmap* pixmap, int px /*= 20*/)
{
    if (!pixmap)
        return QImage();

    const QImage& image = pixmap->toImage();
    QImage* pImage = const_cast<QImage*>(&image);

    const int width = image.width();
    const int height = image.height();

    for (int i = 0; i < width; i += px) {
        for (int j = 0; j < height; j += px) {
            QSize size(px, px);
            if (width - i < px)
                size.setWidth(width - i);
            if (height - j < px)
                size.setHeight(height - j);

            const QPoint topLeft(i, j);
            const QRect rt(topLeft, size);

            //            qInfo()<<"--------->>i:"<< i << "  j:" << j << "  rt:" << rt;
            QColor color = pImage->pixelColor(rt.topLeft());
            for (int x = rt.x(); x <= rt.right(); ++x) {
                for (int y = rt.y(); y <= rt.bottom(); ++y)
                    pImage->setPixelColor(x, y, color);
            }
        }
    }

    return image;  // TODO 可优化: 值传递
}

const int ArrowWidth = 10;
const int ArrowHeight = 18;
QPainterPath GetArrowHead(QPoint p1, QPoint p2, const int thickness /*= 10*/)
{
    QLineF base(p1, p2);
    // Create the vector for the position of the base  of the arrowhead
    QLineF temp(QPoint(0, 0), p2 - p1);
    int val = ArrowHeight + thickness * 4;
    if (base.length() < val) {
        val = static_cast<int>(base.length() + thickness * 2);
    }
    temp.setLength(base.length() + thickness * 2 - val);
    // Move across the line up to the head
    QPointF bottomTranslation(temp.p2());

    // Rotate base of the arrowhead
    base.setLength(ArrowWidth + thickness * 2);
    base.setAngle(base.angle() + 90);
    // Move to the correct point
    QPointF temp2 = p1 - base.p2();
    // Center it
    QPointF centerTranslation((temp2.x() / 2), (temp2.y() / 2));

    base.translate(bottomTranslation);
    base.translate(centerTranslation);

    QPainterPath path;
    path.moveTo(p2);
    path.lineTo(base.p1());
    path.lineTo(base.p2());
    path.lineTo(p2);
    return path;
}

QLine GetShorterLine(QPoint p1, QPoint p2, const int thickness /*= 10*/)
{
    QLineF l(p1, p2);
    int val = ArrowHeight + thickness * 4;
    if (l.length() < val) {
        val = static_cast<int>(l.length() + thickness * 2);
    }
    l.setLength(l.length() + thickness * 2 - val);
    return l.toLine();
}

double getScale(QScreen *screen)
{
// or defined(Q_WS_WIN) || defined(Q_WS_X11)
#if defined(Q_OS_WIN) ||  defined(Q_OS_LINUX)
    double scale = screen->logicalDotsPerInch() / 96.0;
    if (scale < 1.25)
        return 1;
    else if (1.25 <= scale && scale < 1.5)
        return 1.25;
    else if (1.5 <= scale && scale < 1.75)
        return 1.5;
    else if (1.75 <= scale && scale < 2)
        return 1.75;
    else if (2 <= scale && scale < 2.25)
        return 2;
    else if (2.25 <= scale && scale < 2.5)
        return 2.25;
    else if (2.5 <= scale && scale < 3)
        return 2.5;
    else if (3 <= scale && scale < 3.5)
        return 3;
    else if (3.5 <= scale && scale < 4)
        return 3.5;
    else
        return scale;
#elif  defined(Q_OS_MAC)
    double scale = screen->logicalDotsPerInch() / 72.0;
    return scale;
#else
    return screen->logicalDotsPerInch() / 96.0;  //
#endif
}

QColor highlightColor(QColor highlight)
{
    highlight.setAlphaF(1);
    return highlight;
}
}


