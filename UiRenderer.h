#pragma once

#include "D2D1_Wrapper.h"

class D2D1_Wrapper;
class Application;
class UiRenderer
{
private:
	ID2D1DCRenderTarget* m_pD2D1RenderTarget{};
	ID2D1SolidColorBrush* m_pD2D1SolidColorBrush{};
	IDWriteTextFormat* m_pTextFormat{};

	const Application* m_pWindow{};
	const D2D1_Wrapper* m_pD2D1Wrapper{};
	HRESULT hr{};
	RECT nWinRc{};

	// Method init. direct2d drawing resources
	void CreateD2D1Resources();

	// Method freeing direct2d drawing resources
	void DiscardD2D1Resources();

public:
	// Method draw window ui background
	void DrawWindowBackground();

	// Method draw window NC-area
	void DrawWindowNcArea();

	// Method redraw WinAPI button
	void OnDrawButton(HDC hDC, const int nButtonState, const int nButtonId, const RECT rc);

public:

	// CTOR
	UiRenderer(const Application* window, const D2D1_Wrapper* d2d1wrap);

	// Method setup render-target for drawing
	void StartDraw();
	
	// Method off render-target drawing mode
	void StopDraw();

	// DTOR
	~UiRenderer();
};