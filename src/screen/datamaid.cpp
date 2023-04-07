﻿// SPDX-License-Identifier:
// SPDX-FileCopyrightText: 2021-2023 XMuli <xmulitech@gmail.com>
//
// Source: https://github.com/XMuli/Flipped
#include "datamaid.h"
#include <QDir>
#include <QFile>
#include <QPainter>
#include <QDateTime>
#include <QByteArray>
#include <QDomElement>
#include <QSvgRenderer>
#include <QPainterPath>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsEffect>
#include "screenshot.h"
#include "../preference/appellation.h"

// dataMaid (必须) 推荐在 .cpp 中实现，不然会有生成新的对象，打印地址不同；下同理
Q_GLOBAL_STATIC_WITH_ARGS(DataMaid, dataMaid, (nullptr))  /*还真的不是他妈的 全局唯一的 单例*/

#define WRITE_INI(root, key, val) \
    m_settings.setValue("/" + root + "/" + key, val)
#define READ_INI(root, key, defVal) \
    m_settings.value("/" + root + "/" + key, defVal)

bool DataIni::setQStrProperty(const QString &proper, const QVariant &val)
{
    // 拆分两行，避免 MSVC Debug 调试时候的概率问题；详细 gitee.com/feiyangqingyun/qtkaifajingyan 185 条 
    const auto& str = proper.toStdString();
    const char* cstr = str.c_str();
    return setProperty(cstr, val);
}

DataIni::DataIni(QObject *parent)
    : QObject(parent)
    , m_settings(qApp->applicationDirPath() + "/config/config.ini", QSettings::IniFormat, nullptr)
{
    resetAllData();
    readFromAllIni();
    //writeToAllIni(true);

    connect(this, &DataIni::sigLanuage, this, [this](QString language) { WRITE_INI(INIT_GENERAL, tgLanuage, language); });
    connect(this, &DataIni::sigFont, this, [this](QString font) { WRITE_INI(INIT_GENERAL, tgFont, font); });
    connect(this, &DataIni::sigAutoRun, this, [this](bool enable) { WRITE_INI(INIT_GENERAL, tgAutoRun, enable); });
    connect(this, &DataIni::sigAsAdmin, this, [this](bool enable) { WRITE_INI(INIT_GENERAL, tgAsAdmin, enable); });
    connect(this, &DataIni::sigLogLevel, this, [this](QString level) { WRITE_INI(INIT_GENERAL, tgLogLevel, level); });
    connect(this, &DataIni::sigAutoUpdate, this, [this](bool enable) { WRITE_INI(INIT_GENERAL, tgAutoUpdate, enable); });

    connect(this, &DataIni::sigBorderStyle, this, [this](QString style) { WRITE_INI(INIT_INTERFACE, tiBorderStyle, style); });
    connect(this, &DataIni::sigBorderColor, this, [this](QString color) { WRITE_INI(INIT_INTERFACE, tiBorderColor, color); });
    connect(this, &DataIni::sigBorderWidth, this, [this](int width) { WRITE_INI(INIT_INTERFACE, tiBorderWidth, width); });
    connect(this, &DataIni::sigCrosshairColor, this, [this](QString color) { WRITE_INI(INIT_INTERFACE, tiCrosshairColor, color); });
    connect(this, &DataIni::sigCrosshairWidth, this, [this](int width) { WRITE_INI(INIT_INTERFACE, tiCrosshairWidth, width); });
    connect(this, &DataIni::sigSmartWindow, this, [this](bool enable) { WRITE_INI(INIT_INTERFACE, tiSmartWindow, enable); });
    connect(this, &DataIni::sigCrosshair, this, [this](bool enable) { WRITE_INI(INIT_INTERFACE, tiCrosshair, enable); });
    connect(this, &DataIni::sigShowCursor, this, [this](bool enable) { WRITE_INI(INIT_INTERFACE, tiShowCursor, enable); });
    connect(this, &DataIni::sigAutoCopy2Clipboard, this, [this](bool enable) { WRITE_INI(INIT_INTERFACE, tiAutoCopy2Clipboard, enable); });

    connect(this, &DataIni::sigImageQuailty, this, [this](int val) { WRITE_INI(INIT_OUTPUT, toImageQuailty, val); });
    connect(this, &DataIni::sigFileName, this, [this](QString str) { WRITE_INI(INIT_OUTPUT, toFileName, str); });
    connect(this, &DataIni::sigConfigPath, this, [this](QString str) { WRITE_INI(INIT_OUTPUT, toConfigPath, str); });
    connect(this, &DataIni::sigQuickSave, this, [this](bool enable) { WRITE_INI(INIT_OUTPUT, toQuickSave, enable); });
    connect(this, &DataIni::sigAutoSave, this, [this](bool enable) { WRITE_INI(INIT_OUTPUT, toAutoSave, enable); });
    connect(this, &DataIni::sigQuickSavePath, this, [this](QString str) { WRITE_INI(INIT_OUTPUT, toQuickSavePath, str); });
    connect(this, &DataIni::sigAutoSavePath, this, [this](QString str) { WRITE_INI(INIT_OUTPUT, toAutoSavePath, str); });

    connect(this, &DataIni::sigWindowShadow, this, [this](bool shadow) { WRITE_INI(INIT_PIN, tpWindowShadow, shadow); });
    connect(this, &DataIni::sigOpacity, this, [this](int val) { WRITE_INI(INIT_PIN, tpOpacity, val); });
    connect(this, &DataIni::sigMaxSize, this, [this](int val) { WRITE_INI(INIT_PIN, tpMaxSize, val); });

    connect(this, &DataIni::sigScrnCapture, this, [this](QString hotkey) { WRITE_INI(INIT_HOTKEYS, thScrnCapture, hotkey); });
    connect(this, &DataIni::sigDelayCapture, this, [this](QString hotkey) { WRITE_INI(INIT_HOTKEYS, thDelayCapture, hotkey); });
    connect(this, &DataIni::sigFullScrnCapture, this, [this](QString hotkey) { WRITE_INI(INIT_HOTKEYS, thFullScreen, hotkey); });
}

void DataIni::readFromAllIni()
{
    const auto lanuage    = READ_INI(INIT_GENERAL, tgLanuage, resetLanuage()).toString();
    const auto font       = READ_INI(INIT_GENERAL, tgFont, resetFont()).toString();
    const auto autoRun    = READ_INI(INIT_GENERAL, tgAutoRun, resetAutoRun()).toBool();
    const auto asAdmin    = READ_INI(INIT_GENERAL, tgAsAdmin, resetAsAdmin()).toBool();
    const auto logLevel   = READ_INI(INIT_GENERAL, tgLogLevel, resetLogLevel()).toString();
    const auto autoUpdate = READ_INI(INIT_GENERAL, tgAutoCheckUpdate, resetAutoUpdate()).toBool();
    setQStrProperty(tgLanuage, lanuage);
    setQStrProperty(tgFont, font);
    setQStrProperty(tgAutoRun, autoRun);
    setQStrProperty(tgAsAdmin, asAdmin);
    setQStrProperty(tgLogLevel, logLevel);
    setQStrProperty(tgAutoUpdate, autoUpdate);

    const auto borderStyle        = READ_INI(INIT_INTERFACE, tiBorderStyle, resetBorderStyle()).toString();
    const auto borderColor        = READ_INI(INIT_INTERFACE, tiBorderColor, resetBorderColor()).toString();
    const auto borderWidth        = READ_INI(INIT_INTERFACE, tiBorderWidth, resetBorderWidth()).toInt();
    const auto crosshairColor     = READ_INI(INIT_INTERFACE, tiCrosshairColor, resetCrosshairColor()).toString();
    const auto crosshairWidth     = READ_INI(INIT_INTERFACE, tiCrosshairWidth, resetCrosshairWidth()).toInt();
    const auto smartWindow        = READ_INI(INIT_INTERFACE, tiSmartWindow, resetSmartWindow()).toBool();
    const auto crosshair          = READ_INI(INIT_INTERFACE, tiCrosshair, resetCrosshair()).toBool();
    const auto showCursor         = READ_INI(INIT_INTERFACE, tiShowCursor, resetShowCursor()).toBool();
    const auto autoCopy2Clipboard = READ_INI(INIT_INTERFACE, tiAutoCopy2Clipboard, resetAutoCopy2Clipboard()).toBool();
    setQStrProperty(tiBorderStyle, borderStyle);
    setQStrProperty(tiBorderColor, borderColor);
    setQStrProperty(tiBorderWidth, borderWidth);
    setQStrProperty(tiCrosshairColor, crosshairColor);
    setQStrProperty(tiCrosshairWidth, crosshairWidth);
    setQStrProperty(tiSmartWindow, smartWindow);
    setQStrProperty(tiCrosshair, crosshair);
    setQStrProperty(tiShowCursor, showCursor);
    setQStrProperty(tiAutoCopy2Clipboard, autoCopy2Clipboard);

    const auto imageQuailty = READ_INI(INIT_OUTPUT, toImageQuailty, resetImageQuailty()).toInt();
    const auto fileName = READ_INI(INIT_OUTPUT, toFileName, resetFileName()).toString();
    const auto configPath = READ_INI(INIT_OUTPUT, toConfigPath, resetConfigPath()).toString();
    const auto quickSave = READ_INI(INIT_OUTPUT, toQuickSave, resetQuickSave()).toBool();
    const auto autoSave = READ_INI(INIT_OUTPUT, toAutoSave, resetAutoSave()).toBool();
    const auto quickSavePath = READ_INI(INIT_OUTPUT, toQuickSavePath, resetQuickSavePath()).toString();
    const auto autoSavePath = READ_INI(INIT_OUTPUT, toAutoSavePath, resetAutoSavePath()).toString();
    setQStrProperty(toImageQuailty, imageQuailty);
    setQStrProperty(toFileName, fileName);
    setQStrProperty(toConfigPath, configPath);
    setQStrProperty(toQuickSave, quickSave);
    setQStrProperty(toAutoSave, autoSave);
    setQStrProperty(toQuickSavePath, quickSavePath);
    setQStrProperty(toAutoSavePath, autoSavePath);

    const auto windowShadow = READ_INI(INIT_PIN, tpWindowShadow, resetWindowShadow()).toBool();
    const auto opacity      = READ_INI(INIT_PIN, tpOpacity, resetOpacity()).toInt();
    const auto maxSize      = READ_INI(INIT_PIN, tpMaxSize, resetMaxSize()).toInt();
    setQStrProperty(tpWindowShadow, windowShadow);
    setQStrProperty(tpOpacity, opacity);
    setQStrProperty(tpMaxSize, maxSize);

    const auto scrnCapture     = READ_INI(INIT_HOTKEYS, thScrnCapture, resetScrnCapture()).toString();
    const auto delayCapture    = READ_INI(INIT_HOTKEYS, thDelayCapture, resetDelayCapture()).toString();
    const auto fullScrnCapture = READ_INI(INIT_HOTKEYS, thFullScreen, resetFullScrnCapture()).toString();
    setQStrProperty(thScrnCapture, scrnCapture);
    setQStrProperty(thDelayCapture, delayCapture);
    setQStrProperty(thFullScreen, fullScrnCapture);
}

void DataIni::writeToAllIni(const bool bReset)
{
    writeToGeneralIni(bReset);
    writeToInterfaceIni(bReset);
    writeToOutputIni(bReset);
    writeToPinIni(bReset);
    writeToHotkeysIni(bReset);
}

void DataIni::writeToGeneralIni(const bool bReset)
{
    if (bReset)  resetGeneral();
    WRITE_INI(INIT_GENERAL, tgLanuage, m_lanuage);
    WRITE_INI(INIT_GENERAL, tgFont, m_font);
    WRITE_INI(INIT_GENERAL, tgAutoRun, m_autoRun);
    WRITE_INI(INIT_GENERAL, tgAsAdmin, m_asAdmin);
    WRITE_INI(INIT_GENERAL, tgLogLevel, m_logLevel);
    WRITE_INI(INIT_GENERAL, tgAutoUpdate, m_autoUpdate);
}

void DataIni::writeToInterfaceIni(const bool bReset)
{
    if (bReset)  resetInterface();
    WRITE_INI(INIT_INTERFACE, tiBorderStyle, m_borderStyle);
    WRITE_INI(INIT_INTERFACE, tiBorderColor, m_borderColor);
    WRITE_INI(INIT_INTERFACE, tiBorderWidth, m_borderWidth);
    WRITE_INI(INIT_INTERFACE, tiCrosshairColor, m_crosshairColor);
    WRITE_INI(INIT_INTERFACE, tiCrosshairWidth, m_crosshairWidth);
    WRITE_INI(INIT_INTERFACE, tiSmartWindow, m_smartWindow);
    WRITE_INI(INIT_INTERFACE, tiCrosshair, m_crosshair);
    WRITE_INI(INIT_INTERFACE, tiShowCursor, m_showCursor);
    WRITE_INI(INIT_INTERFACE, tiAutoCopy2Clipboard, m_autoCopy2Clipboard);
}

void DataIni::writeToOutputIni(const bool bReset)
{
    if (bReset)  resetOutput();
    WRITE_INI(INIT_OUTPUT, toImageQuailty, m_imageQuailty);
    WRITE_INI(INIT_OUTPUT, toFileName, m_fileName);
    WRITE_INI(INIT_OUTPUT, toConfigPath, m_configPath);
    WRITE_INI(INIT_OUTPUT, toQuickSave, m_quickSave);
    WRITE_INI(INIT_OUTPUT, toAutoSave, m_autoSave);
    WRITE_INI(INIT_OUTPUT, toQuickSavePath, m_quickSavePath);
    WRITE_INI(INIT_OUTPUT, toAutoSavePath, m_autoSavePath);
}

void DataIni::writeToPinIni(const bool bReset)
{
    if (bReset)  resetPin();
    WRITE_INI(INIT_PIN, tpWindowShadow, m_windowShadow);
    WRITE_INI(INIT_PIN, tpOpacity, m_opacity);
    WRITE_INI(INIT_PIN, tpMaxSize, m_maxSize);
}

void DataIni::writeToHotkeysIni(const bool bReset)
{
    if (bReset)  resetHotkeys();
    WRITE_INI(INIT_HOTKEYS, thScrnCapture, m_scrnCapture);
    WRITE_INI(INIT_HOTKEYS, thDelayCapture, m_delayCapture);
    WRITE_INI(INIT_HOTKEYS, thFullScreen, m_fullScrnCapture);
}

void DataIni::resetAllData()
{
    resetGeneral();
    resetInterface();
    resetOutput();
    resetPin();
    resetHotkeys();
}

void DataIni::resetGeneral()
{
    resetLanuage();
    resetFont();
    resetAutoRun();
    resetAsAdmin();
    resetLogLevel();
    resetAutoUpdate();
}

void DataIni::resetInterface()
{
    resetBorderStyle();
    resetBorderColor();
    resetBorderWidth();
    resetCrosshairColor();
    resetCrosshairWidth();
    resetSmartWindow();
    resetCrosshair();
    resetShowCursor();
    resetAutoCopy2Clipboard();
}

void DataIni::resetOutput()
{
    resetImageQuailty();
    resetFileName();
    resetConfigPath();
    resetQuickSave();
    resetAutoSave();
    resetQuickSavePath();
    resetAutoSavePath();
}

void DataIni::resetPin()
{
    resetWindowShadow();
    resetOpacity();
    resetMaxSize();
}

void DataIni::resetHotkeys()
{
    resetScrnCapture();
    resetDelayCapture();
    resetFullScrnCapture();
}

DataMaid::DataMaid(QObject *parent)
{
    m_dataIni.resetAllData();
}

QVariant DataMaid::paraValue(const char *key)
{
    return m_dataIni.property(key);
}

QVariant DataMaid::paraValue(const QString key)
{
    return paraValue(key.toLocal8Bit().data());;
}

void DataMaid::setParaValue(const char *key, const QVariant &val)
{
    m_dataIni.setProperty(key, val);
}

void DataMaid::setParaValue(const QString key, const QVariant& val)
{
    setParaValue(key.toLocal8Bit().data(), val);
}

void DataMaid::readFromAllIni()
{
    m_dataIni.readFromAllIni();
}

void DataMaid::writeToAllIni(const bool bReset)
{
    m_dataIni.writeToAllIni(bReset);
}

DataMaid* DataMaid::instance()
{
    return dataMaid();
}

void DataMaid::writeToGeneralIni(const bool bReset)
{
    m_dataIni.writeToGeneralIni(bReset);
}

void DataMaid::writeToInterfaceIni(const bool bReset)
{
    m_dataIni.writeToInterfaceIni(bReset);
}

void DataMaid::writeToOutputIni(const bool bReset)
{
    m_dataIni.writeToOutputIni(bReset);
}

void DataMaid::writeToPinIni(const bool bReset)
{
    m_dataIni.writeToPinIni(bReset);
}

void DataMaid::writeToHotkeysIni(const bool bReset)
{
    m_dataIni.writeToHotkeysIni(bReset);
}

double DataMaid::scale(QScreen* screen)
{
    double scale = 0;
#if defined(Q_OS_WIN) ||  defined(Q_OS_LINUX)          // or defined(Q_WS_WIN) || defined(Q_WS_X11)
    scale = screen->logicalDotsPerInch() / 96.0;
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
    scale = screen->logicalDotsPerInch() / 72.0;
    return scale;
#endif
}

/*!
 * \brief changeSVGColor 修改 .svg 的填充色
 * \param path   .svg 文件路径
 * \param shape  欲修改的图形， 通常是 path、偶尔是 rect 等
 * \param color  填充的颜色
 * \param size   svg 伸缩后的尺寸
 * \return 一个 QIcon 对象
 */
QIcon DataMaid::ChangeSVGColor(QString path, QString shape, QColor color, QSize size)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QByteArray baData = file.readAll();

    QDomDocument doc;
    doc.setContent(baData);
    QDomElement elem = doc.documentElement(); // const 和 值传递
    SetAttrRecur(elem, shape, "fill", color.name());

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

void DataMaid::SetAttrRecur(QDomElement& elem, QString strtagname, QString strattr, QString strattrval)
{
    // if it has the tagname then overwritte desired attribute
    if (elem.tagName().compare(strtagname) == 0)
        elem.setAttribute(strattr, strattrval);

    // loop all children
    for (int i = 0; i < elem.childNodes().count(); i++) {
        if (!elem.childNodes().at(i).isElement())
            continue;

        QDomElement t = elem.childNodes().at(i).toElement();
        SetAttrRecur(t, strtagname, strattr, strattrval);
    }
}

const QPixmap* DataMaid::smoothMosaic(QPixmap* pixmap, int radius)
{
    if (!pixmap)
        return nullptr;

    radius = qMax<int>(10, radius);
    QGraphicsBlurEffect* blur = new QGraphicsBlurEffect;
    blur->setBlurRadius(radius);
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(*pixmap);
    item->setGraphicsEffect(blur);

    QGraphicsScene scene;
    scene.addItem(item);

    QPainter painter(pixmap);
    const double DPI = ScreenShot::GetDevicePixelRatio();
    const QRect tRt(pixmap->rect().topLeft() / DPI, pixmap->rect().size() / DPI);
    scene.render(&painter, tRt, QRectF());

    blur->setBlurRadius(radius + 2);
    // multiple repeat for make blur effect stronger
    scene.render(&painter, tRt, QRectF());

    return pixmap;
}

const QImage DataMaid::pixlelatedMosaic(QPixmap* pixmap, int px)
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
            //qDebug()<<"--------->>i:"<< i << "  j:" << j << "  rt:" << rt;
            QColor color = pImage->pixelColor(rt.topLeft());
            for (int x = rt.x(); x <= rt.right(); ++x) {
                for (int y = rt.y(); y <= rt.bottom(); ++y)
                    pImage->setPixelColor(x, y, color);
            }
        }
    }

    return image;
}

void DataMaid::setAutoRun()
{
#if defined(Q_OS_WIN)
    QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    const bool bAutoRun = DATAMAID->paraValue(tgAutoRun).toBool();
    if (bAutoRun) {
        QString strAppPath = QDir::toNativeSeparators(qApp->applicationFilePath());
        strAppPath.replace(QChar('/'), QChar('\\'), Qt::CaseInsensitive);
        reg.setValue("Flipped", strAppPath);
    } else {
        reg.setValue("Flipped", "");
    }
#else
    // TODO 2023.03.31:
#endif
}
const QString DataMaid::formatToFileName(const QString name)
{
    auto fileName = name.trimmed();
    auto first = fileName.indexOf("$");
    auto second = fileName.lastIndexOf("$");
    auto nameTime = fileName.mid(first + 1, second - first - 1);
    auto finalyName = fileName.replace(first, second - first + 1, QDateTime::currentDateTime().toString(nameTime));

    return finalyName;
}