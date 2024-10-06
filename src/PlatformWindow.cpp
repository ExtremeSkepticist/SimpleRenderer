#include "PlatformWindow.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

namespace SR
{
	namespace Platform
	{
		PlatformWindow::PlatformWindow(uint16_t frameWidth, uint16_t frameHeight, gsl::not_null<gsl::wzstring> className, gsl::not_null<HINSTANCE> hInstance)
			: mFrameWidth(minFrameWidth)
			, mFrameHeight(minFrameHeight)
			, mClassName(className)
			, mHwnd(nullptr)
			, mHinstance(hInstance)
		{
			Expects(frameWidth >= minFrameWidth && frameHeight >= minFrameHeight);

			mFrameWidth = frameWidth;
			mFrameHeight = frameHeight;

			Ensures(mFrameWidth == frameWidth && mFrameHeight == frameHeight);
		}

		bool PlatformWindow::Initialize()
		{
			Expects(mHwnd == nullptr);

			WNDCLASSEX windowClassEx = { 0, };
			windowClassEx.cbSize = sizeof(WNDCLASSEX);
			windowClassEx.style = CS_HREDRAW | CS_VREDRAW;
			windowClassEx.lpfnWndProc = WndProc;
			windowClassEx.hInstance = mHinstance;
			windowClassEx.lpszClassName = mClassName;

			ATOM registeredFlag = RegisterClassEx(&windowClassEx);
			assert(registeredFlag != 0, "Failed to register WNDCLASSEX");

			RECT rec = { 0, 0, mFrameWidth, mFrameHeight };
			BOOL hasRecAdjusted = AdjustWindowRect(&rec, WS_OVERLAPPEDWINDOW, FALSE);
			assert(hasRecAdjusted, "Failed to adjust window rect");

			mHwnd = CreateWindow(mClassName.get(), nullptr
				, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
				, CW_USEDEFAULT, CW_USEDEFAULT, rec.right - rec.left, rec.bottom - rec.top
				, nullptr, nullptr, mHinstance, nullptr);

			const bool isInitSucceeded = mHwnd != nullptr;
			Ensures(isInitSucceeded);

			return isInitSucceeded;
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct = { 0, };
	HDC hDevice = { 0, };

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