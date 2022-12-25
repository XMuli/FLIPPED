﻿/*******************************************************************
 * Copyright (c) 2021-2022 偕臧  All rights reserved.
 *
 * Author: XMuli <xmulitech@gmail.com>
 * GitHub: https://github.com/XMuli
 * Blog:   https://xmuli.tech
 *
 * Date: 2022.06.22
 * Description: 绘画工具栏 下面那个具体参数 ToolBar
 ******************************************************************/
#include "parameterbar.h"
#include "../xglobal.h"
#include "../widget/xhorizontalline.h"
#include "../widget/xverticalline.h"
#include "../widget/xcombobox.h"
#include "../core/xlog.h"
#include "base/blurwidget.h"
#include <QColor>
#include <QSize>
#include <QPainter>
#include <QBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QToolButton>
#include <QButtonGroup>
#include <QAbstractButton>

//test
#include <QDebug>

ParameterBar::ParameterBar(Qt::Orientations orien, QWidget* parent)
    : QWidget(parent)
    , m_scal(XHelper::instance().getScale())
    , m_blur(nullptr)
    , m_orien(orien)
    , m_layout(nullptr)
    , m_colorBar(new ColorParaBar())
    , m_rectBar(nullptr)
    , m_ellipseBar(nullptr)
    , m_mosaicBar(nullptr)
    , m_arrowBar(nullptr)
    , m_lienWidthBar(nullptr)
    , m_serialnumberShape(nullptr)
    , m_serialnumberType(nullptr)
{
    initUI();

#if defined(Q_OS_WIN) ||  defined(Q_OS_LINUX)
    m_blur = new BlurWidget(this);
#else
//    setAttribute(Qt::WA_TranslucentBackground, true);

//    setWindowOpacity(0.8);

//    QPalette pal = palette();  // 在其 parent 中效果会透明，但是
//    pal.setColor(QPalette::Window, QColor(255, 255, 255, 0.7 * 255));
//    setPalette(pal);
#endif

    connect(m_colorBar, &ColorParaBar::sigColorChange, this, &ParameterBar::sigSelColor);
}

void ParameterBar::setBlurBackground(const QPixmap &pix, double blurRadius)
{
    if (!m_blur)
        return;

    m_blur->setPixmap(pix, blurRadius);
    m_blur->lower();
}

void ParameterBar::addWidget(QWidget *w)
{
    if (m_layout && w)
        m_layout->addWidget(w);
}

void ParameterBar::addSpacer()
{
    if (!m_layout)
        return;

    if (m_orien == Qt::Horizontal)
        m_layout->addWidget(new XVerticalLine(B_SPACER_LINE_HEIGHT * m_scal, this));
    else
        m_layout->addWidget(new XHorizontalLine(B_SPACER_LINE_HEIGHT * m_scal, this));
}

void ParameterBar::creatorParaBar(QPointer<ManageBar>& manageBar, QString& path, QStringList items)
{
    if (!manageBar)
        manageBar = new ManageBar(m_orien, this);

    QMap<QString, QString> map;
    for (int i = 0; i < items.size(); ++i) {
        map.insert(QString("tb%1").arg(i), path + items[i] + ".svg");
    }

    QButtonGroup* group = new QButtonGroup(this);
    group->setExclusive(true); // toolBtn 互斥

    auto it = map.begin();
    for (int i = 0; i < items.size(); ++i) {
        QToolButton* tb = new QToolButton();
        tb->setIconSize(QSize(ICON_WIDTH * m_scal, ICON_WIDTH * m_scal));
        tb->setFixedSize(QSize(ICON_WIDTH * m_scal, ICON_WIDTH * m_scal));
        tb->setObjectName(it.key());
        tb->setIcon(QIcon(it.value()));
        tb->setToolButtonStyle(Qt::ToolButtonIconOnly);
        tb->setAutoRaise(true);   // 自动浮动模式
        tb->setCheckable(true);
        tb->setChecked(false);
        if (i == 0) {  // 第一个为默认选中
            tb->setChecked(true);
            tb->setIcon(XHelper::instance().ChangeSVGColor(it.value(), "rect", XHelper::instance().borderColor(), QSize(ICON_WIDTH, ICON_WIDTH) * XHelper::instance().getScale()));
        }

        tb->setProperty("path", it.value());
        group->addButton(tb);
        manageBar->addWidget(tb);
        it++;
    }

    void (QButtonGroup::* sigFun)(QAbstractButton*) = &QButtonGroup::buttonReleased;
    connect(group, sigFun, this, &ParameterBar::onTBReleased);
}

void ParameterBar::initRectBar()
{
    QString path(":/resources/tool_para/rectangles/");
    QStringList list = { "rectangle",  "rectangle_fill" };

    creatorParaBar(m_rectBar, path, list);
}

void ParameterBar::initEllipseBar()
{
    QString path(":/resources/tool_para/ellipses/");
    QStringList list = { "ellipse",  "ellipse_fill" };
    creatorParaBar(m_ellipseBar, path, list);
}

void ParameterBar::initMosaicBar()
{
    QString path(":/resources/tool_para/mosaics/");
    QStringList list = { "smooth", "pixelated" };
    creatorParaBar(m_mosaicBar, path, list);
}

void ParameterBar::initArrowBar()
{
    QString path(":/resources/tool_para/arrows/");
    QStringList list = { "arrow_1", "arrow_2", "arrow_3", "arrow_4" };
    creatorParaBar(m_arrowBar, path, list);
}

void ParameterBar::initLineWidthBar()
{
    QString path(":/resources/tool_para/line_width/");
    QStringList list = { "line_width_1", "line_width_2", "line_width_3" };
    creatorParaBar(m_lienWidthBar, path, list);
}

void ParameterBar::initSerialnumberBar()
{
    QString path(":/resources/tool_para/serialnumber/");
    QStringList list = { "rectangle_fill", "ellipse_fill" };
    creatorParaBar(m_serialnumberShape, path, list);

    QStringList listType = { "numbers", "letters" };
    creatorParaBar(m_serialnumberType, path, listType);
}

void ParameterBar::removeAllBar()
{
    if (!m_layout)
        return;

    QWidgetList list;

    for (int i = 0; i < m_layout->count(); ++i) {
        QWidget* widget = m_layout->itemAt(i)->widget();

        if (widget)
            list << widget;
    }

    for (auto& it : list) {
        bool isXLine = false;
        if (m_orien == Qt::Horizontal) {
            auto t = qobject_cast<XVerticalLine*>(it);
            if (t)
                isXLine = true;

        } else {
            auto t = qobject_cast<XHorizontalLine*>(it);
            if (t)
                isXLine = true;
        }

        m_layout->removeWidget(it);
        it->setVisible(false);

        if (isXLine)  // 若是分割线直接销毁
            it->deleteLater();
    }
}

void ParameterBar::onEnableDraw(bool enable)
{
    setVisible(enable);
}

void ParameterBar::onSelShape(DrawShape shape, bool checked)
{
    removeAllBar();

    #define ADDWIDGET_AND_SHOE(w, bAddSpacer) \
        addWidget(w); \
        w->setVisible(true); \
        if (bAddSpacer) \
            addSpacer();

    if (shape == DrawShape::NoDraw) {
        XLOG_INFO("[shape] is DrawShape::NoDraw.");
    } else if (shape == DrawShape::Rectangles) {
        ADDWIDGET_AND_SHOE(m_rectBar, true);
        ADDWIDGET_AND_SHOE(m_lienWidthBar, true);
        ADDWIDGET_AND_SHOE(m_colorBar, false);
    } else if (shape == DrawShape::Ellipses) {
        ADDWIDGET_AND_SHOE(m_ellipseBar, true);
        ADDWIDGET_AND_SHOE(m_lienWidthBar, true);
        ADDWIDGET_AND_SHOE(m_colorBar, false);
    } else if (shape == DrawShape::LineWidth) {
        ADDWIDGET_AND_SHOE(m_lienWidthBar, true);
        ADDWIDGET_AND_SHOE(m_colorBar, false);
    } else if (shape == DrawShape::Arrows) {
        ADDWIDGET_AND_SHOE(m_arrowBar, true);
        ADDWIDGET_AND_SHOE(m_lienWidthBar, true);
        ADDWIDGET_AND_SHOE(m_colorBar, false);
    } else if (shape == DrawShape::Pen) {
        ADDWIDGET_AND_SHOE(m_lienWidthBar, true);
        ADDWIDGET_AND_SHOE(m_colorBar, false);
    } else if (shape == DrawShape::Mosaics) {
        ADDWIDGET_AND_SHOE(m_mosaicBar, true);
        ADDWIDGET_AND_SHOE(m_lienWidthBar, false);
    } else if (shape == DrawShape::Text) {
        ADDWIDGET_AND_SHOE(m_ellipseBar, true);  // TODO 2022.06.27： 需要替换为 font size 大小的 combobox
        ADDWIDGET_AND_SHOE(m_colorBar, false);
    } else if (shape == DrawShape::SerialNumber) {
        ADDWIDGET_AND_SHOE(m_serialnumberShape, true);
        ADDWIDGET_AND_SHOE(m_serialnumberType, true);
        ADDWIDGET_AND_SHOE(m_colorBar, false);
    } else {
        XLOG_ERROR("[shape] is not any enumeration.");
    }

    setVisible(checked);
    adjustSize();
    update();
}

void ParameterBar::enterEvent(QEvent* event)
{
    setCursor(Qt::ArrowCursor);
}

void ParameterBar::resizeEvent(QResizeEvent *event)
{
    if (m_blur)
        m_blur->setGeometry(0, 0, width(), height());

    return QWidget::resizeEvent(event);
}

void ParameterBar::paintEvent(QPaintEvent *event)
{
#if defined(Q_OS_WIN) ||  defined(Q_OS_LINUX)
    QWidget::paintEvent(event);
#else
    Q_UNUSED(event)
//    updateToolBtnIcon();
    QPainter pa(this);
    pa.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    pa.setPen(Qt::NoPen);
    pa.setBrush(QColor(255, 255, 255, 0.7 * 255));

    const int round = 4;
    pa.drawRoundedRect(contentsRect().adjusted(1, 1, -1, -1), round, round);
#endif
}

void ParameterBar::onTBReleased(QAbstractButton* btn)
{
    const auto& parent = btn->parentWidget();
    if (!btn || !parent)
        return;

    for (auto& it : parent->findChildren<QToolButton *>()) {
        QString path = it->property("path").value<QString>();
        it->setIconSize(QSize(ICON_WIDTH, ICON_WIDTH) * XHelper::instance().getScale());

        auto ptr = qobject_cast<QToolButton*>(btn);
        if (it == ptr) {
            it->setIcon(XHelper::instance().ChangeSVGColor(path, "rect", XHelper::instance().borderColor(), QSize(ICON_WIDTH, ICON_WIDTH) * XHelper::instance().getScale()));

            //enum class DrawShape {
            //    NoDraw,
            //    Rectangles,
            //    Ellipses,
            //    Line,
            //    Arrows,
            //    Pen,
            //    Mosaics,
            //    Text,
            //    SerialNumber
            //};


            //m_colorBar;
            //m_serialBar;

            //m_rectBar;
            //m_ellipseBar;
            //m_mosaicBar;
            //m_arrowBar;
            //m_lienWidthBar;
            // TODO: 此为当前点击选中这个
            QString name = it->objectName();
//            int n = it->objectName().right(1).toInt();
            
            //const QString cTb1("tb1");
            //const QString cTb2("tb2");
            //const QString cTb3("tb3");

            DrawShape shap = DrawShape::NoDraw;

            if (parent == m_rectBar) {
                shap = DrawShape::Rectangles;
            } else if (parent == m_ellipseBar) {
                shap = DrawShape::Ellipses;
            } else if (parent == m_mosaicBar) {
                shap = DrawShape::Mosaics;
            } else if (parent == m_arrowBar) {
                shap = DrawShape::Arrows;
                //if (name == cTb1) {
                //} else if (name == cTb2) {
                //} else if (name == cTb3) {
                //} else {
                //}
            } else if (parent == m_lienWidthBar) {
                shap = DrawShape::LineWidth;   // or Pen
            } else if (parent == m_serialnumberShape || parent == m_serialnumberType) {
                if (parent == m_serialnumberShape) shap = DrawShape::SerialNumber;
                if (parent == m_serialnumberType) shap = DrawShape::SerialNumberType;
            } else {
            }

            emit sigParaBtnId(shap, ptr);

        } else {
            it->setIcon(QIcon(path));
        }
    }
}

void ParameterBar::initUI()
{
    if (m_orien == Qt::Horizontal)
        m_layout = new QHBoxLayout(this);
    else
        m_layout = new QVBoxLayout(this);

    initRectBar();
    initEllipseBar();
    initMosaicBar();
    initArrowBar();
    initLineWidthBar();
    initSerialnumberBar();

    if (m_colorBar)
        m_colorBar->setVisible(false);

    m_layout->setMargin(0);
    //const int bbMarginHor = BAR_MARGIN_HOR;
    //int bbMarginVer = BAR_MARGIN_VER;
    //if (m_colorBar->isVisible()) {
    //    bbMarginVer = BAR_MARGIN_VER_HAS_COLOR_PARA;
    //    m_layout->setContentsMargins(bbMarginHor, 0, 0, 0);
    //} else {
    //    m_layout->setContentsMargins(bbMarginHor, bbMarginVer, bbMarginHor, bbMarginVer);
    //}

    setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(PB_ITEM_SPACE);

    m_layout->addWidget(m_rectBar);
    addSpacer();
    m_layout->addWidget(m_ellipseBar);
    addSpacer();
    m_layout->addWidget(m_mosaicBar);
    addSpacer();
    m_layout->addWidget(m_arrowBar);
    addSpacer();
    m_layout->addWidget(m_lienWidthBar);
    addSpacer();
    m_layout->addWidget(m_serialnumberShape);
    addSpacer();
    m_layout->addWidget(m_serialnumberType);
    addSpacer();
    m_layout->addWidget(m_colorBar);

//    adjustSize();  // 布局后重新计算一下大小尺寸
//    resize(size().width() + bbMarginHor * 2, size().height() + bbMarginVer * 2);
}
