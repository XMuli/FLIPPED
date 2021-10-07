﻿//
// Created by xmuli on 2021/10/08.
//

#include "tray.h"
#include "winresetbtn.h"
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QIcon>
#include "QCoreApplication"

Tray::Tray(QObject *parent)
    : QObject(parent)
	, m_screenShot(nullptr)
	, m_showWinConfig(nullptr)
	, m_quit(nullptr)
	, m_menuTary(nullptr)
	, m_sysTary(nullptr)
	, m_winMain(nullptr)
	, m_winFallScn(nullptr)
	, m_hkScnShot(nullptr)
{
	init();
}

Tray::~Tray()
{
	m_winMain->deleteLater();
}

void Tray::init()
{
	m_screenShot = new QAction(tr("ScreenShot"), this);
	m_showWinConfig = new QAction(tr("Config"), this);
	m_quit = new QAction(tr("Quit"), this);

	m_menuTary = new QMenu();
	m_menuTary->addAction(m_screenShot);
	m_menuTary->addAction(m_showWinConfig);
	m_menuTary->addSeparator();
	m_menuTary->addAction(m_quit);

	m_sysTary = new QSystemTrayIcon(this);
	m_sysTary->setIcon(QIcon(":/resources/PicShot_32.svg"));
	m_sysTary->setToolTip(tr("PicShot"));
	m_sysTary->setContextMenu(m_menuTary);
	m_sysTary->setVisible(true);

	m_winMain = new WinMain();
	m_winFallScn = new WinFullScreen();

	connect(m_screenShot, &QAction::triggered, this, &Tray::onScreenShot);
	connect(m_showWinConfig, &QAction::triggered, this, &Tray::onShowWinConfig);
	connect(m_quit, &QAction::triggered, this, &Tray::deleteLater); // 代替退出槽

	m_hkScnShot = new QHotkey(QKeySequence("ctrl+alt+t"), true, qApp);//The hotkey will be automatically registered
	qDebug() << "Is Registered: " << m_hkScnShot->isRegistered();
	QObject::connect(m_hkScnShot, &QHotkey::activated, this, &Tray::onScreenShot);
}

void Tray::onScreenShot()
{
	// TODO 2021-10-08 模拟按下快捷键或后续的封装函数
	
	m_winFallScn->display();
	m_winFallScn->getBasePixmap();
	m_winFallScn->show();
}

void Tray::onShowWinConfig(bool checked)
{
	Q_UNUSED(checked)
	m_winMain->show();
}
