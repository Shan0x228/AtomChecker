#pragma once

#pragma warning(disable : 4996)
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include "AtomHTTP.hpp"
#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <Uxtheme.h>
#include <dwmapi.h>
#include <vsstyle.h>
#include <vssym32.h>
#include <string>
#include <vector>
#include <codecvt>
#include <sstream>
#include <algorithm>
#include "D2D1_Wrapper.h"

class D2D1_Wrapper;
class UiRenderer;
class Application
{

// Variables:
private:
	HWND m_hAppMainWindow{};

	D2D1_Wrapper* d2d1_wrap{};
	UiRenderer* render{};

	const std::wstring m_szwAppTitle{L"Atom Checker"};
	const int m_nAppWidth{ 380 }, m_nAppHeight{ 450 };

	const int m_nMargins{ 0 }, m_nMarginTitleBar{ 31 };

	bool IsFocused{};

	HDC hWinDC{};

	friend class UiRenderer;

	std::wstring LoadedBase{};
	std::vector<std::wstring> BaseData{};

	Shkolnik::net::AtomHTTP* HttpRequest{};

// Methods:
private:
	// Method redraw WinAPI controls
	void OnDrawControls(const DRAWITEMSTRUCT* pDis);

	// Method creates controls
	void CreateUiControls();

	// Method set font for all controls
	void SetUiFont();

	// Method handle form create message
	void OnWindow_Create();

	// Method handle close message
	void OnWindow_Destroy();

	// Method handle paint message
	void OnWindow_Paint();

	// Native window object registration
	bool RegisterNativeWindow();

	// Static application message processing procedure
	static LRESULT CALLBACK ApplicationProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Main window message processing procedure
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Processing NC-area hittesting
	LRESULT ComputeSectorOfWindow(HWND hwnd, WPARAM, LPARAM lParam, int nCaptionHeight);

	// Method processing ui events
	void DispatchUiEvent(const int nButtonId);

	// Method parse login:password
	std::pair<std::wstring, std::wstring> SplitLoginData(std::wstring & data);

private:
	// On load button click event
	void OnLoadButton_Click();

	// On start button click event
	void OnStartButton_Click();

	// On stop button click event
	void OnStopButton_Click();

// Controls
private:
	enum UI_CTL_ID
	{
		LOAD_BTN,
		START_BTN,
		STOP_BTN,
		TEXT_LOG,
		SYS_LINK
	};

	HWND hLoadButton{}, hStartButton{}, hStopButton{}, hListBox{}, hSysLink{};

public:
	// CTOR
	Application();

	// Start msg proc
	int Run();

	// DTOR
	~Application();
};