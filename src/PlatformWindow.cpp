#include <limits>
#include <Windows.h>

#include "PlatformWindow.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

namespace sr
{
	namespace platform
	{
		static constexpr uint16_t minFrameWidth = 800;
		static constexpr uint16_t minFrameHeight = 600;
		static constexpr double aspectRatio = 4.0f / 3;

		static_assert(static_cast<double>(minFrameWidth) / minFrameHeight - aspectRatio <= std::numeric_limits<double>::epsilon()
			, "Aspect ratio should remain 4:3. Did you change min frame width/height?");

		PlatformWindow::PlatformWindow(const uint16_t frameWidth, const uint16_t frameHeight
			, const gsl::not_null<gsl::cwzstring> className, const gsl::not_null<HINSTANCE> hInstance) noexcept
			: mFrameWidth(0)
			, mFrameHeight(0)
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
			if (registeredFlag != 0)
			{
				return false;
			}

			RECT rec = { 0, 0, mFrameWidth, mFrameHeight };
			if (!AdjustWindowRect(&rec, WS_OVERLAPPEDWINDOW, FALSE))
			{
				return false;
			}

			mHwnd = CreateWindow(mClassName, nullptr
				, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
				, CW_USEDEFAULT, CW_USEDEFAULT, rec.right - rec.left, rec.bottom - rec.top
				, nullptr, nullptr, mHinstance, nullptr);

			return mHwnd != nullptr;
		}

		bool PlatformWindow::DisplayWindow(const int32_t nCmdShow) const noexcept
		{
			Expects(mHwnd != nullptr);

			return ShowWindow(mHwnd, nCmdShow);
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