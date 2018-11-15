#include "Application.h"

// Native window object registration
bool Application::RegisterNativeWindow()
{
	this->d2d1_wrap = new D2D1_Wrapper{};

	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = nullptr;
	wc.lpfnWndProc = Application::ApplicationProc;
	wc.lpszClassName = this->m_szwAppTitle.c_str();
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc))
		return false;

	if (this->m_hAppMainWindow = CreateWindowEx(
		0,
		this->m_szwAppTitle.c_str(),
		this->m_szwAppTitle.c_str(),
		WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX,
		(GetSystemMetrics(SM_CXSCREEN) - this->m_nAppWidth) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - this->m_nAppHeight) / 2,
		this->m_nAppWidth,
		this->m_nAppHeight,
		nullptr, nullptr, wc.hInstance, this); !this->m_hAppMainWindow)
		return false;

	this->render = new UiRenderer{ this, this->d2d1_wrap };

	const MARGINS margins{this->m_nMargins, this->m_nMargins, this->m_nMarginTitleBar, this->m_nMargins};
	DwmExtendFrameIntoClientArea(this->m_hAppMainWindow, &margins);

	UpdateWindow(this->m_hAppMainWindow);
	ShowWindow(this->m_hAppMainWindow, SW_SHOW);

	return true;
}

// Static application message processing procedure
LRESULT Application::ApplicationProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Application* pApp;
	if (uMsg == WM_NCCREATE)
	{
		pApp = static_cast<Application*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetLastError(0);
		if (!SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp)))
		{
			if (GetLastError() != 0)
				return false;
		}
	}
	else
	{
		pApp = reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	if (pApp)
	{
		pApp->m_hAppMainWindow = hwnd;
		return pApp->WindowProc(hwnd, uMsg, wParam, lParam);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Main window message processing procedure
LRESULT Application::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	LRESULT lr{};
	auto DwmProcessed = DwmDefWindowProc(hwnd, uMsg, wParam, lParam, &lr);

	switch (uMsg)
	{
		case WM_CREATE:
		{
			this->OnWindow_Create();
		}
		break;

		case WM_PAINT:
		{
			BeginPaint(hwnd, &ps);
			this->hWinDC = ps.hdc;
			this->OnWindow_Paint();
			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DRAWITEM:
		{
			this->OnDrawControls(reinterpret_cast<DRAWITEMSTRUCT*>(lParam));
		}
		break;

		case WM_COMMAND:
		{
			this->DispatchUiEvent(LOWORD(wParam));
		}
		break;

		case WM_ACTIVATE:
		{
			this->IsFocused = static_cast<bool>(LOWORD(wParam));
			//if (!this->IsFocused)
			//	InvalidateRect(hwnd, nullptr, false);
		}
		break;

		case WM_NCCALCSIZE:
		{
			auto client_area_needs_calculating = static_cast<bool>(wParam);

			if (client_area_needs_calculating) {
				auto parameters = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);

				auto& requested_client_area = parameters->rgrc[0];
				requested_client_area.right -= GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
				requested_client_area.left += GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
				requested_client_area.bottom -= GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);

				return 0;
			}
		}
		break;

		case WM_NCHITTEST:
		{
			if (lr == 0)
			{
				auto dpi = GetDpiForSystem();
				auto user_scaling = static_cast<float>(dpi) / 96.0f;
				auto caption_height = this->m_nMarginTitleBar;
				auto corrected_height = static_cast<int>(std::ceilf(user_scaling * caption_height));
				lr = this->ComputeSectorOfWindow(hwnd, wParam, lParam, corrected_height);
				if (lr != HTNOWHERE)
					return lr;
			}
		}
		break;

		case WM_LBUTTONDOWN:
		{
			if (GetFocus() == this->hListBox)
				SetFocus(hwnd);
		}
		break;

		case WM_DESTROY:
		{
			this->OnWindow_Destroy();
			PostQuitMessage(0);
		}
		break;

		case WM_NOTIFY:
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case NM_CLICK:
				case NM_RETURN:
				{
					PNMLINK pNMLink = (PNMLINK)lParam;
					LITEM item = pNMLink->item;

					if ((((LPNMHDR)lParam)->hwndFrom == this->hSysLink) && (item.iLink == 0))
						ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);

					break;
				}
			}
			break;
		}
		break;
	}
	
	if (!DwmProcessed)
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	else return lr;
}

// CTOR
Application::Application()
{
}

// Start msg proc
int Application::Run()
{
	if (!this->RegisterNativeWindow())
		return EXIT_FAILURE;

	MSG msg{};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}

// DTOR
Application::~Application()
{
	if (this->render != nullptr)
		delete this->render;
	this->render = nullptr;

	if (this->d2d1_wrap != nullptr)
		delete this->d2d1_wrap;
	this->d2d1_wrap = nullptr;
}
