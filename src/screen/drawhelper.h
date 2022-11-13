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
#ifndef XDRAW_H
#define XDRAW_H

#include "../core/isingleton.h"
#include "../preference/appellation.h"
#include <QObject>
#include <QPen>
#include <QRect>
#include <QFont>
#include <QPainterPath>
#include <QApplication>
#include <QScreen>
#include <QColor>
#include <QCoreApplication>
#include <QApplication>
#include <QSettings>
#include <QString>
//#include <Format>

class QLine;
class QPoint;
class QDomElement;

namespace XC {
	//enum ScrnStatus {
	//	// 基础的几种状态
	//	SS_WaitBase,                                // 基础的等待状态（未有，和已有矩形局域）
	//	SS_SelectBase,                              // 基础的选中状态
	//	SS_MoveBase,                                // 基础的移动状态
	//	SS_DrawBase,                                // 基础的绘画状态
	//	SS_StretchBase,                             // 基础的拉伸状态
	//};

	//enum ScreenStatus {                                  // ------------（矩形区域）------------
	//	Auto,     // 自动检测
	//	Custom,   // 手动拖曳

	//	Left,
	//	Right,
	//	Top,
	//	Bottom,
	//	TopLeft,
	//	TopRight,
	//	BottomLeft,
	//	BottomRight,

	//	Width = Left | Right,                    // 左、右
	//	Height = Top | Bottom,                   // 上、下
	//	Edge = Width | Height,                   // 任意一边（左、右、上、下）
	//	TLAndBR = TopLeft | BottomRight,         // 斜对角（左上、右下）
	//	TRAndBL = TopRight | BottomLeft,         // 斜对角（右上、左下）
	//	Corner = TLAndBR | TRAndBL,              // 斜任意一斜角（左上、右下、右上、左下）
	//	Frame = Edge | Corner,                   // 矩形任意一边或一斜角（左、右、上、下；左上、右下、右上、左下）

	//	UnknowCursorType                         // 未知
	//};
	//Q_DECLARE_FLAGS(CursorTypes, ScreenStatus)     // 枚举 ScrnType 生成宏 CursorTypes
	//	Q_DECLARE_OPERATORS_FOR_FLAGS(CursorTypes)   // 重载宏 ScrnType 的 |() 函数

	// C++11 新增带作用域的枚举，用 enum class  或enum struct（两者等价）声明。
	// https://blog.csdn.net/luckysym/article/details/1666114
	enum class DrawState {
		Draw,
		Move,
		Delete,
		Wateing,
		Unknow
	};

	enum class DrawShape {
		NoDraw,
		Rectangles,
		Ellipses,
        LineWidth,   // 特殊、线宽度
        Arrows,
        Pen,
        Mosaics,
		Text,
        SerialNumber
	};

    enum class ToolBarOffset {
        TBO_Left,
		TBO_Middle,
		TBO_Right,

        TBO_Top = TBO_Left,
        TBO_Bottom = TBO_Right
    };

	enum class LineEnds {
		EmptyToEmpty,
		EmptyToRecesedArrow,
		DotToRecesedArrow,
		EmptyToArrow,
		DotToArrow,
		EmptyToDot,
		DotToEmpty,
		DotToDot,
		RecesedArrowToRecesedArrow,
		ArrowToArrow
	};
}

using namespace XC;
// NoDraw,
// Rectangles,
// Ellipses,
// Line,
// Arrows,
// Pen,
// Mosaics,
// Text,
// SerialNumber

struct  XDrawStep
{
	//new refactor
	// base element ------------------
    QPoint p1;                                  // 起点
    QPoint p2;                                  // 终点
    QRect rt;                                   // 初始绘画位置: 由 p1、p2 构成
    DrawShape shape = DrawShape::NoDraw;        // 绘画形状
	int bStyele = 0;                            // refactor 图形的样式, 取代之前  bFill
    int tbIdx = 0;                              // 某个绘画类型的样式: 为 ParameterBar 的子序号
    int idxLevel = -1;                          // 所处的序号等级，亦 z 轴，越大越顶层;  此项待重构
    static int totalIdx;                        // 所有已绘画类型 的总的序号，越大越上层

    //QPainterPath path;                          // 存储所有绘画类型

	// color, pen width
	QPen pen = QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin); // 默认红色，画笔宽度为 2px
	QBrush brush = QBrush(Qt::NoBrush);         // 画刷

	// Rectangles --------------------
	// Ellipses ----------------------
	// Line / Arrows -----------------
	LineEnds lineEnds = LineEnds::EmptyToEmpty;

	// Arrows ------------------------
    // Pen ---------------------------
	QVector<QPoint> custPath;                   // 手绘路径

    // Mosaics -----------------------
	int mscPx = 3;                              // 马赛克默认模糊块

    // Text --------------------------
    QString text;                               // 文字内容
	QFont font = QFont("KaiTi", 14);            // 字体

    // SerialNumber ------------------

    void clear() {
        p1 = QPoint();
        p2 = QPoint();
        rt = QRect();
        //shape = DrawShape::NoDraw; // 若为鼠标松开执行，则会无法继续绘画抽象图形
        //pen = QPen(Qt::red);
        //penWidth = 2;
        //brush = QBrush(Qt::NoBrush);
        //brushWidth = 1;
        //transparency = 1;
        //rX = 8;
        //rY = 8;
        //lineEnd = LineEnds::EmptyToEmpty;
        //lineDashe = LineDashes::SolidLine;
        custPath.clear();
        //text = "==Test Text==";
        //font = QFont();
        //fontSize = 16;
        //mscPx = 3;

        idxLevel = -1;
        //g_index = -1;  永不重置

    }
};

// ------------------------
class XHelper : public QObject
{
    Q_OBJECT
public:
    static XHelper& instance() {
        static XHelper instance;
        return instance;
    }

    XHelper(XHelper&&) = delete;
    XHelper(const XHelper&) = delete;
    void operator= (const XHelper&) = delete;
private:
    XHelper();
    virtual ~XHelper() = default;

public:
    double getScale(QScreen* screen = QApplication::primaryScreen());

    void setBoardStyle(const QString style) { m_boardStyle = style; }
    const QString boardStyle() { return m_boardStyle; }
    void setBorderColor(QColor color) { m_borderColor = color; }
    const QColor borderColor() { return m_borderColor; }
    void setBorderWidth(const int width) { m_borderWidth = width; }
    const int borderWidth() { return m_borderWidth; }
    void setCrosshairColor(QColor color) { m_crosshairColor = color; }
    const QColor crosshairColor() { return m_crosshairColor; }
    void setCrosshairWidth(const int width) { m_crosshairWidth = width; }
    const int crosshairWidth() { return m_crosshairWidth; }
    bool smartWindow() const { return m_enableSmartWindow; }
    void setSmartWindow(bool enable) { m_enableSmartWindow = enable; }
    bool crosshair() const { return m_enableCrosshair; }
    void setCrosshair(bool enable) { m_enableCrosshair = enable; }
    bool showCursor() const { return m_enableShowCursor; }
    void setShowCursor(bool enable) { m_enableShowCursor = enable; }
    bool autoCpoyClip() const { return m_enableAutoCpoyClip; }
    void setAutoCpoyClip(bool enable) { m_enableAutoCpoyClip = enable; }

    int imgQuailty() const { return m_imgQuailty; }
    void setImgQuailty(int val) { m_imgQuailty = val; }
    const QString formatToName(const QString str = XHelper::instance().path(toFileName));

	QIcon ChangeSVGColor(QString path, QString shape, QColor color, QSize size);
    void SetAttrRecur(QDomElement& elem, QString strtagname, QString strattr, QString strattrval);

    // Mosaics draw
    const QPixmap* SetMosaicSmooth(QPixmap* pixmap, int px);         // 毛玻璃马赛克
    const QImage SetMosaicPixlelated(QPixmap* pixmap, int px = 20);  // 像素级马赛克

    // Arrow Line draw
    QPainterPath GetArrowHead(QPoint p1, QPoint p2, const int thickness = 10);
    QLine GetShorterLine(QPoint p1, QPoint p2, const int thickness = 10);

    // tabGeneral
    // tabInterface

    // tabOutput
	QString path(const QString key) const;
    void setPath(const QString key, const QString val);

    // tabPin
    bool winShadow() const;
    void setWinShadow(bool enable);

    int pinOpacity() const;
    void setPinOpacity(int opacity);
    int pinMaxSize() const;
    void setPinMaxSize(int val);

signals:
    void sigChangeWinShadow(bool enable);
    void sigChangeOpacity(int opacity);
    void sigChangeMaxSize(int val);

private:
    QString m_boardStyle;
    QColor m_borderColor;                      // 边框
    int m_borderWidth;
    QColor m_crosshairColor;                   // 边框
    int m_crosshairWidth;
    bool m_enableSmartWindow;
	bool m_enableCrosshair;
    bool m_enableShowCursor;
    bool m_enableAutoCpoyClip;

    int m_imgQuailty;
	QMap<QString, QString> m_path;

    bool m_winShadow;
    int m_pinOpacity;
    int m_pinMaxSize;
};

// ------------------------
// 创建全局静态 单例 的对象, 就不浪费生命重新创建了，  路径后面替换为 ConfigLocation
Q_GLOBAL_STATIC_WITH_ARGS(QSettings, settingIni, (qApp->applicationDirPath() + "/config/config.ini", QSettings::IniFormat));
//Q_GLOBAL_STATIC(XHelper, insXHelp);

 // perference UI config
const QString INIT_GENERAL("General");                   // 初始化 常规
const QString INIT_INTERFACE("Interface");               // 初始化 界面
const QString INIT_OUTPUT("Output");                     // 初始化 输出
const QString INIT_PIN("Pin");                           // 初始化 贴图
const QString INIT_HOTKEYS("Hotkeys");                   // 初始化 快捷键

#endif // XDRAW_H
