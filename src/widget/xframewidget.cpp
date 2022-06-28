﻿/*******************************************************************
 * Copyright (c) 2021-2022 偕臧  All rights reserved.
 *
 * Author: XMuli <xmulitech@gmail.com>
 * GitHub: https://github.com/XMuli
 * Blog:   https://xmuli.tech
 *
 * Date: 2022.06.22
 * Description: 基础底部 widget
 ******************************************************************/
#include "xframewidget.h"
#include "../xglobal.h"
#include <QColor>
#include <QPainter>
#include <QGraphicsEffect>
#include <QPen>

XFrameWidget::XFrameWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);      // 去掉标题栏
    setAttribute(Qt::WA_TranslucentBackground);   // 设置透明，自绘画为圆角矩形
    //setWindowOpacity(0.7);
    //PerformanceHint = 0x00,
    //    QualityHint = 0x01,
    //    AnimationHint = 0x02

    QGraphicsBlurEffect* blureffect = new QGraphicsBlurEffect(this);
    blureffect->setBlurRadius(1);	//数值越大，越模糊
    blureffect->setBlurHints(QGraphicsBlurEffect::QualityHint);
    this->setGraphicsEffect(blureffect);


}

void XFrameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter pa(this);
    pa.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QColor colBrush("#131313");
    colBrush.setAlphaF(0.6);
    pa.setBrush(colBrush);
    QColor colPen("#FFFFFF");
    colPen.setAlphaF(0.1);
    QPen pen(colPen, 1);
    pa.setPen(pen);

    const int margin1 = 1;
    pa.drawRoundedRect(contentsRect().adjusted(margin1, margin1, -margin1, -margin1), CW_RADIRS, CW_RADIRS);

    colPen.setNamedColor("#000000");
    colPen.setAlphaF(0.1);
    pa.setPen(colPen);
    pa.setBrush(Qt::NoBrush);
    const int margin2 = 0;
    pa.drawRoundedRect(contentsRect().adjusted(margin2, margin2, -margin2, -margin2), CW_RADIRS, CW_RADIRS);
}
