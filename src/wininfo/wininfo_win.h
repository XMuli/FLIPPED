﻿#ifndef WININFO_WIN_H
#define WININFO_WIN_H
#include "iwininfo.h"
#include "../core/tsingleton.h"

#include <windows.h>
#include <atlstr.h>
#include <vector>
// 窗口信息_win 版本
class WinInfoWin : public TSingleton<WinInfoWin>// , IWinInfo
{
private:
	WinInfoWin();

public:
    void getAllWinInfoCache();
	HWND getAllWinInfoRealTime(POINT pt);

	void setWindowsFilter(HWND hWnd);
	RECT getWindowsRectFromPoint(POINT pt, BOOL bSmartDetection = TRUE);

//
protected:
	friend class TSingleton<WinInfoWin>;
//
//protected:
public:
	static BOOL CALLBACK EnumRealTimeWindowsProc(HWND hWnd, LPARAM lParam);
	static BOOL CALLBACK EnumChildRealTimeWindowsProc(HWND hWnd, LPARAM lParam);

    static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM level);
    static BOOL CALLBACK EnumChildWindowsProc(HWND hWnd, LPARAM level);

    static BOOL WindowsContainsPoint(HWND hWnd, POINT pt);
	static BOOL getRectFromCache(POINT pt, RECT& rect);
    static BOOL WindowsFilter(HWND hWnd);
    static CString getWindowPath(DWORD processId);
    static CString windowPath2Name(CString path);

//    error: LNK2005: "protected: static struct HWND__ * WinInfoWin::m_hWndTarget" (?m_hWndTarget@WinInfoWin@@1PAUHWND__@@A) already defined in mocs_compilation.cpp.obj

    // 静态成员变量只能在 cpp 中初始化，坑了一会 https://stackoverflow.com/questions/40991522
//private:
    static HWND m_hWndTarget;
    static HWND m_hWndFilter;
    static std::vector<WinInfo> m_vWinInfo;
};


#endif // WININFO_WIN_H