#include <limits>
#include <Windows.h>

#include "PlatformWindow.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

namespace SR
{
	namespace Platform
	{
		constexpr uint16_t minFrameWidth = 800;
		constexpr uint16_t minFrameHeight = 600;
		constexpr double aspectRatio = 4.0f / 3;

		static_assert(static_cast<double>(minFrameWidth) / minFrameHeight - aspectRatio <= std::numeric_limits<double>::epsilon()
			, "Aspect ratio should remain 4:3. Did you change min frame width/height?");

		PlatformWindow::PlatformWindow(const uint16_t frameWidth, const uint16_t frameHeight
			, const gsl::not_null<gsl::cwzstring> className, const gsl::not_null<HINSTANCE> hInstance) noexcept
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
			Expects(mFrameWidth >= minFrameWidth && mFrameHeight >= minFrameHeight);

			const WNDCLASSEX windowClassEx = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW
				, WndProc, 0, 0, mHinstance, nullptr, nullptr, nullptr, mClassName, mClassName };
			const ATOM registeredFlag = RegisterClassEx(&windowClassEx);
			assert(registeredFlag != 0);

			RECT rec = { 0, 0, mFrameWidth, mFrameHeight };
			const BOOL hasRecAdjusted = AdjustWindowRect(&rec, WS_OVERLAPPEDWINDOW, FALSE);
			assert(hasRecAdjusted);

			mHwnd = CreateWindow(mClassName, nullptr
				, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
				, CW_USEDEFAULT, CW_USEDEFAULT, rec.right - rec.left, rec.bottom - rec.top
				, nullptr, nullptr, mHinstance, nullptr);

			const bool isInitSucceeded = mHwnd != nullptr;
			return isInitSucceeded;
		}

		bool PlatformWindow::DisplayWindow(const int32_t nCmdShow) const noexcept
		{
			Expects(mHwnd != nullptr);

			const bool hasShowWindowSucceeded = ShowWindow(mHwnd, nCmdShow);
			return hasShowWindowSucceeded;
		}

		const HWND& PlatformWindow::GetHwnd() const noexcept
		{
			return mHwnd;
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