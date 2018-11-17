#include "UiRenderer.h"

// Method draw window ui background
void UiRenderer::DrawWindowBackground()
{
	this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(0.219f, 0.403f, 0.839f, 1.0f));
	this->m_pD2D1RenderTarget->FillRectangle(D2D1::RectF(
		0, 
		this->m_pWindow->m_nMarginTitleBar, 
		this->m_pWindow->m_nAppWidth, 
		this->m_pWindow->m_nAppWidth - this->m_pWindow->m_nMarginTitleBar), this->m_pD2D1SolidColorBrush);
}

// Method draw window NC-area
void UiRenderer::DrawWindowNcArea()
{
	this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(0.921f, 0.231f, 0.352f, 1.0f));
	this->m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	this->m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	this->m_pD2D1RenderTarget->DrawText(this->m_pWindow->m_szwAppTitle.c_str(),
		this->m_pWindow->m_szwAppTitle.length(),
		this->m_pTextFormat,
		D2D1::RectF(4, 0, 100, this->m_pWindow->m_nMarginTitleBar),
		this->m_pD2D1SolidColorBrush);
}

// Method redraw WinAPI button
void UiRenderer::OnDrawButton(HDC hDC, const int nButtonState, const int nButtonId, const RECT rc)
{
	this->m_pD2D1RenderTarget->BindDC(hDC, &rc);
	this->m_pD2D1RenderTarget->BeginDraw();
	this->m_pD2D1RenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White, 0.0f));

	if (nButtonState & ODS_SELECTED)
		this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
	else if (nButtonState & ODS_DISABLED)
		this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(1.0f, 0.247f, 0.203f, 1.0f));
	else
		this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Orange));

	this->m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	this->m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	switch (nButtonId)
	{
		case static_cast<int>(Application::UI_CTL_ID::LOAD_BTN):
		{
			this->m_pD2D1RenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, rc.right, rc.bottom), this->m_pD2D1SolidColorBrush);
			
			if (nButtonState & ODS_SELECTED)
				this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
			else if (nButtonState & ODS_DISABLED)
				this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(D2D1::ColorF::WhiteSmoke));
			else
				this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(0.294f, 0.396f, 0.517f, 1.0f));

			this->m_pD2D1RenderTarget->DrawText(L"Load base", 10, this->m_pTextFormat, D2D1::RectF(0.0f, 0.0f, rc.right, rc.bottom), this->m_pD2D1SolidColorBrush);
		}
		break;

		case static_cast<int>(Application::UI_CTL_ID::START_BTN):
		{
			this->m_pD2D1RenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, rc.right, rc.bottom), this->m_pD2D1SolidColorBrush);
			
			if (nButtonState & ODS_SELECTED)
				this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
			else if (nButtonState & ODS_DISABLED)
				this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(D2D1::ColorF::WhiteSmoke));
			else
				this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(0.294f, 0.396f, 0.517f, 1.0f));

			this->m_pD2D1RenderTarget->DrawText(L"Start", 6, this->m_pTextFormat, D2D1::RectF(0.0f, 0.0f, rc.right, rc.bottom), this->m_pD2D1SolidColorBrush);
		}
		break;

		case static_cast<int>(Application::UI_CTL_ID::STOP_BTN):
		{
			this->m_pD2D1RenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, rc.right, rc.bottom), this->m_pD2D1SolidColorBrush);

			if (nButtonState & ODS_SELECTED)
				this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
			else if (nButtonState & ODS_DISABLED)
				this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(D2D1::ColorF::WhiteSmoke));
			else
				this->m_pD2D1SolidColorBrush->SetColor(D2D1::ColorF(0.294f, 0.396f, 0.517f, 1.0f));

			this->m_pD2D1RenderTarget->DrawText(L"Stop", 5, this->m_pTextFormat, D2D1::RectF(0.0f, 0.0f, rc.right, rc.bottom), this->m_pD2D1SolidColorBrush);
		}
		break;
	}
	this->m_pD2D1RenderTarget->EndDraw();
}