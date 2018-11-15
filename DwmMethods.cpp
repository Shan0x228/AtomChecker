#include "Application.h"

// Processing NC-area hittesting
LRESULT Application::ComputeSectorOfWindow(HWND hwnd, WPARAM, LPARAM lParam, int nCaptionHeight)
{
	// Acquire the window rect
	RECT window_rectangle;
	GetWindowRect(hwnd, &window_rectangle);

	auto offset = 10;

	POINT cursor_position{
		GET_X_LPARAM(lParam),
		GET_Y_LPARAM(lParam)
	};

	if (cursor_position.y < window_rectangle.top + offset && cursor_position.x < window_rectangle.left + offset) return HTTOPLEFT;
	if (cursor_position.y < window_rectangle.top + offset && cursor_position.x > window_rectangle.right - offset) return HTTOPRIGHT;
	if (cursor_position.y > window_rectangle.bottom - offset && cursor_position.x > window_rectangle.right - offset) return HTBOTTOMRIGHT;
	if (cursor_position.y > window_rectangle.bottom - offset && cursor_position.x < window_rectangle.left + offset) return HTBOTTOMLEFT;

	if (cursor_position.x > window_rectangle.left && cursor_position.x < window_rectangle.right) {
		if (cursor_position.y < window_rectangle.top + offset) return HTTOP;
		else if (cursor_position.y > window_rectangle.bottom - offset) return HTBOTTOM;
	}
	if (cursor_position.y > window_rectangle.top && cursor_position.y < window_rectangle.bottom) {
		if (cursor_position.x < window_rectangle.left + offset) return HTLEFT;
		else if (cursor_position.x > window_rectangle.right - offset) return HTRIGHT;
	}

	if (cursor_position.x > window_rectangle.left && cursor_position.x < window_rectangle.right) {
		if (cursor_position.y < window_rectangle.top + nCaptionHeight) return HTCAPTION;
	}

	return HTNOWHERE;
}