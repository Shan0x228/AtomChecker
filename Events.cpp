#include "Application.h"

// On load button click event
void Application::OnLoadButton_Click()
{
	this->BaseData.clear();
	this->LoadedBase.clear();
	SendMessage(this->hListBox, LB_RESETCONTENT, 0, 0);
	ListBox_AddString(this->hListBox, L".:|LOG|:.");


	HANDLE hFile = CreateFile(L"C:\\Users\\Shan0x\\source\\repos\\AtomChecker\\30k.txt", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	DWORD dwFileSize = GetFileSize(hFile, nullptr);

	std::string buffer{};
	buffer.resize(dwFileSize);

	ReadFile(hFile, &buffer[0], dwFileSize, nullptr, nullptr);
	buffer.erase(remove(begin(buffer), end(buffer), 0), end(buffer));

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	this->LoadedBase = converter.from_bytes(buffer);

	CloseHandle(hFile);

	// TODO...
	std::wistringstream data(this->LoadedBase);
	std::wstring temp{};
	while (getline(data, temp, L'\r\n'))
		this->BaseData.push_back(temp);
		

	for (const auto &x : this->BaseData)
		ListBox_AddString(this->hListBox, std::wstring(L"- " + x).c_str());
}

// On start button click event
void Application::OnStartButton_Click()
{
	// ÌÍÎÃÎÏÎÒÎ×ÍÎÑÒÜ ÒÓÒ!
	EnableWindow(this->hStartButton, false);
	EnableWindow(this->hStopButton, true);

	SendMessage(this->hListBox, LB_RESETCONTENT, 0, 0);
	std::pair<std::wstring, std::wstring> login_pass{};
	std::wstring data{};
	for (auto& x : this->BaseData)
	{
		data.clear();
		login_pass = this->SplitLoginData(x);
		data += L"username=" + login_pass.first + L"&" + L"password=" + login_pass.second;
		HttpRequest = new Shkolnik::net::AtomHTTP{ L"127.0.0.1", 8080 };
		
		try
		{
			HttpRequest->SetupRequest(L"/", Shkolnik::net::AtomHTTP::HTTP::POST);
		}
		catch (const std::exception& ex)
		{
			Beep(1, 1);
		}

		HttpRequest->AddHeader(L"Host: 127.0.0.1");
		HttpRequest->AddHeader(L"Content-Type: application/x-www-form-urlencoded");
		HttpRequest->AddHeader(L"Accept: */*");
		HttpRequest->AddHeader(L"Content-Length: " + std::to_wstring(data.length()));

		HttpRequest->AddRequestBody(data);
		HttpRequest->SendRequest();

		std::wstring res = HttpRequest->GetResponseBody();

		std::wstring::size_type invalid_pos = res.find(L"Bad login or password!");
		if (invalid_pos == std::wstring::npos)
		{
			// ÄÀÍÍÛÅ ÂÀËÈÄÍÛÅ
			ListBox_AddString(this->hListBox, std::wstring(L"VALID DATA -> " + login_pass.first + L":" + login_pass.second).c_str());

			delete this->HttpRequest;
			this->HttpRequest = nullptr;

			break;
		}
		ListBox_AddString(this->hListBox, std::wstring(L"INVALID DATA -> " + login_pass.first + L":" + login_pass.second).c_str());
		delete this->HttpRequest;
		this->HttpRequest = nullptr;
	}
	EnableWindow(this->hStopButton, false);
	EnableWindow(this->hStartButton, true);
}

// On stop button click event
void Application::OnStopButton_Click()
{
	// TODO...
	MessageBox(this->m_hAppMainWindow, L"StopBtn!", L"OnClick!", MB_ICONINFORMATION);
}

// Method processing ui events
void Application::DispatchUiEvent(const int nButtonId)
{
	switch (nButtonId)
	{
		case Application::UI_CTL_ID::LOAD_BTN:
		{
			this->OnLoadButton_Click();
		}
		break;

		case Application::UI_CTL_ID::START_BTN:
		{
			this->OnStartButton_Click();
		}
		break;

		case Application::UI_CTL_ID::STOP_BTN:
		{
			this->OnStopButton_Click();
		}
		break;
	}
}

// Method parse login:password
std::pair<std::wstring, std::wstring> Application::SplitLoginData(std::wstring & data)
{
	std::wstring::size_type d = data.find(L"\n");
	if (d != std::wstring::npos)
		data.erase(d, 1);

	auto x = data.find(L":");
	std::wstring login = data.substr(0, x);
	std::wstring password = data.substr(x + 1);
	Beep(1, 1);
	return {
		login,
		password
	};
}

// Method redraw WinAPI controls
void Application::OnDrawControls(const DRAWITEMSTRUCT * pDis)
{
	this->render->OnDrawButton(pDis->hDC, pDis->itemState, pDis->CtlID, pDis->rcItem);
}

// Method creates controls
void Application::CreateUiControls()
{
	this->hLoadButton = CreateWindow(
		L"BUTTON",
		L"Load base",
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
		0, 50,
		this->m_nAppWidth - 16, 25,
		this->m_hAppMainWindow,
		reinterpret_cast<HMENU>(Application::UI_CTL_ID::LOAD_BTN), nullptr, nullptr
	);

	this->hStartButton = CreateWindow(
		L"BUTTON",
		L"Start",
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
		0, 100,
		this->m_nAppWidth - 16, 25,
		this->m_hAppMainWindow,
		reinterpret_cast<HMENU>(Application::UI_CTL_ID::START_BTN), nullptr, nullptr
	);

	this->hStopButton = CreateWindow(
		L"BUTTON",
		L"Stop",
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
		0, 150,
		this->m_nAppWidth - 16, 25,
		this->m_hAppMainWindow,
		reinterpret_cast<HMENU>(Application::UI_CTL_ID::STOP_BTN), nullptr, nullptr
	);
	EnableWindow(this->hStopButton, false);

	this->hListBox = CreateWindow(
		L"LISTBOX",
		L".:|LOG|:.",
		WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOSEL,
		0, 200,
		this->m_nAppWidth - 16,
		260, this->m_hAppMainWindow, reinterpret_cast<HMENU>(Application::UI_CTL_ID::TEXT_LOG),
		nullptr, nullptr
	);

	ListBox_AddString(this->hListBox, L".:|LOG|:.");

	this->hSysLink = CreateWindow(WC_LINK,
		L"<A HREF=\"https://www.youtube.com/channel/UCmJT3IfHtpFJyln2UdABBKg\">YouTube</A>",
		WS_VISIBLE | WS_CHILD | WS_TABSTOP | LWS_TRANSPARENT,
		170, 8, 48, 15,
		this->m_hAppMainWindow, reinterpret_cast<HMENU>(Application::UI_CTL_ID::SYS_LINK), nullptr, nullptr
	);
	/*LITEM item = { 0 };
	item.iLink = 0;
	item.mask = LIF_ITEMINDEX | LIF_STATE;
	item.state = LIS_DEFAULTCOLORS;
	item.stateMask = LIS_DEFAULTCOLORS;
	SendMessage(this->hSysLink, LM_SETITEM, 0, (LPARAM)&item);*/
}

// Font...
BOOL CALLBACK EnumChildProc(
	_In_ HWND   hwnd,
	_In_ LPARAM lParam
)
{
	SendMessage(hwnd, WM_SETFONT, static_cast<WPARAM>(lParam), TRUE);
	return true;
}

// Method set font for all controls
void Application::SetUiFont()
{
	HFONT hFont = CreateFont(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 0, DEFAULT_PITCH, L"Arial");

	EnumChildWindows(this->m_hAppMainWindow, EnumChildProc, reinterpret_cast<LPARAM>(hFont));
}

// Method handle form create message
void Application::OnWindow_Create()
{
	// TODO...
	this->CreateUiControls();
	this->SetUiFont();

	// Force window to recalculate NC area
	SetWindowPos(this->m_hAppMainWindow, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
}

// Method handle close message
void Application::OnWindow_Destroy()
{
	// TODO...
}

// Method handle paint message
void Application::OnWindow_Paint()
{
	this->render->StartDraw();

	// DRAWING UI!
	this->render->DrawWindowNcArea();
	this->render->DrawWindowBackground();
	// DRAWING UI!

	this->render->StopDraw();
}