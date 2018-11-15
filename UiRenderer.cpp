#include "UiRenderer.h"

// Method init. direct2d drawing resources
void UiRenderer::CreateD2D1Resources()
{
	GetClientRect(this->m_pWindow->m_hAppMainWindow, &this->nWinRc);

	D2D1_SIZE_U size = D2D1::SizeU(
		this->m_pWindow->m_nAppWidth,
		this->m_pWindow->m_nAppHeight
	);

	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE_PREMULTIPLIED),
		0,
		0,
		D2D1_RENDER_TARGET_USAGE_NONE,
		D2D1_FEATURE_LEVEL_DEFAULT
	);

	if (this->hr = this->m_pD2D1Wrapper->m_pD2D1Factory->CreateDCRenderTarget(&props, &this->m_pD2D1RenderTarget); !SUCCEEDED(this->hr))
		ExitProcess(EXIT_FAILURE);

	if (this->hr = this->m_pD2D1RenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &this->m_pD2D1SolidColorBrush); !SUCCEEDED(this->hr))
		ExitProcess(EXIT_FAILURE);

	if (this->hr = this->m_pD2D1Wrapper->m_pDWriteFactory->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		13,
		L"", //locale
		&m_pTextFormat
	); !SUCCEEDED(this->hr))
		ExitProcess(EXIT_FAILURE);
}

// Method freeing direct2d drawing resources
void UiRenderer::DiscardD2D1Resources()
{
	D2D1_Wrapper::FreeD2D1Obj(this->m_pD2D1SolidColorBrush);
	D2D1_Wrapper::FreeD2D1Obj(this->m_pTextFormat);
	D2D1_Wrapper::FreeD2D1Obj(this->m_pD2D1RenderTarget);
}

// CTOR
UiRenderer::UiRenderer(const Application* window, const D2D1_Wrapper* d2d1wrap)
	:m_pWindow(window), m_pD2D1Wrapper(d2d1wrap)
{
	this->CreateD2D1Resources();
}

// Method setup render-target for drawing
void UiRenderer::StartDraw()
{
	this->m_pD2D1RenderTarget->BindDC(this->m_pWindow->hWinDC, &this->nWinRc);
	this->m_pD2D1RenderTarget->BeginDraw();
	this->m_pD2D1RenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White, 0.0f));
}

// Method off render-target drawing mode
void UiRenderer::StopDraw()
{
	this->m_pD2D1RenderTarget->EndDraw();
}

// DTOR
UiRenderer::~UiRenderer()
{
	this->DiscardD2D1Resources();
}
