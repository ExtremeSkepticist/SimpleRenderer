#include <limits>
#include <Windows.h>

#include "PlatformWindow.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

namespace sr
{
	namespace platform
	{
		PlatformWindow::PlatformWindow(const uint16_t frameWidth, const uint16_t frameHeight
			, const gsl::not_null<gsl::cwzstring> className, const gsl::not_null<HINSTANCE> hInstance, int nCmdShow) noexcept
			: mFrameWidth(minFrameWidth)
			, mFrameHeight(minFrameHeight)
			, mClassName(className)
			, mHwnd(nullptr)
			, mHinstance(hInstance)
		{
			Expects(mHwnd == nullptr);
			Expects(frameWidth >= minFrameWidth && frameHeight >= minFrameHeight);

			mFrameWidth = frameWidth;
			mFrameHeight = frameHeight;
			
			const WNDCLASSEX windowClassEx = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW
				, WndProc, 0, 0, mHinstance, nullptr, nullptr, nullptr, mClassName, mClassName };
			const ATOM registeredFlag = RegisterClassEx(&windowClassEx);
			assert(registeredFlag != 0);

			RECT rec = { 0, 0, mFrameWidth, mFrameHeight };
			const BOOL isRectAdjusted = AdjustWindowRect(&rec, WS_OVERLAPPEDWINDOW, FALSE);
			assert(isRectAdjusted);

			mHwnd = CreateWindow(mClassName, nullptr
				, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
				, CW_USEDEFAULT, CW_USEDEFAULT, rec.right - rec.left, rec.bottom - rec.top
				, nullptr, nullptr, mHinstance, nullptr);
			
			Ensures(mHwnd != nullptr);
			Ensures(mFrameWidth >= minFrameWidth && mFrameHeight >= minFrameHeight);
		
			ShowWindow(mHwnd, nCmdShow);
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	PAINTSTRUCT paintStruct = { nullptr, };
	HDC hDevice = { nullptr, };

	switch (message)
	{
	case WM_PAINT:
		hDevice = BeginPaint(hWnd, &paintStruct);
		EndPaint(hWnd, &paintStruct);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}