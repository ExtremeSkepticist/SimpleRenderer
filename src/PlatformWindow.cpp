#include <limits>
#include <Windows.h>

#include "UserInput.h"
#include "PlatformWindow.h"

//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

namespace sr
{
	namespace platform
	{
		PlatformWindow::PlatformWindow(const uint16_t frameWidth, const uint16_t frameHeight
			, const gsl::not_null<gsl::cwzstring> className, int nCmdShow) noexcept
			: mFrameWidth(frameWidth)
			, mFrameHeight(frameHeight)
			, mClassName(className)
			, mHwnd(nullptr)
		{
			Expects(mHwnd == nullptr);
			Expects(mFrameWidth >= minFrameWidth && mFrameHeight >= minFrameHeight);

			const HINSTANCE hInstance = GetModuleHandle(nullptr);

			WNDCLASSEX windowClassEx = { 0, };
			windowClassEx.cbSize = sizeof(WNDCLASSEX);
			windowClassEx.style = CS_HREDRAW | CS_VREDRAW;
			windowClassEx.lpfnWndProc = WndProc;
			windowClassEx.cbClsExtra = 0;
			windowClassEx.cbWndExtra = 0;
			windowClassEx.hInstance = hInstance;
			windowClassEx.hIcon = nullptr;
			windowClassEx.hCursor = nullptr;
			windowClassEx.hbrBackground = nullptr;
			windowClassEx.lpszMenuName = mClassName;
			windowClassEx.lpszClassName = mClassName;

			const ATOM registeredFlag = RegisterClassEx(&windowClassEx);
			assert(registeredFlag != 0);

			RECT rec = { 0, };
			rec.left = 0;
			rec.top = 0;
			rec.right = mFrameWidth;
			rec.bottom = mFrameHeight;

			const BOOL isRectAdjusted = AdjustWindowRect(&rec, WS_OVERLAPPEDWINDOW, FALSE);
			assert(isRectAdjusted);

			mHwnd = CreateWindow(mClassName, nullptr
				, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
				, CW_USEDEFAULT, CW_USEDEFAULT, rec.right - rec.left, rec.bottom - rec.top
				, nullptr, nullptr, hInstance, nullptr);
			
			Ensures(mHwnd != nullptr);

			// SetWindowLongPtr(mHwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

			ShowWindow(mHwnd, nCmdShow);
		}

		PlatformWindow::~PlatformWindow() noexcept = default;

		void PlatformWindow::Update() noexcept
		{

		}

		UserInput& PlatformWindow::GetUserInput()
		{
			static UserInput userInput = {};

			return userInput;
		}

		void PlatformWindow::OnPressedKey(WPARAM key) noexcept
		{
			switch (tolower(key))
			{
			case 'q':
				GetUserInput().mPressedAscend = true;
				break;
			case 'e':
				GetUserInput().mPressedDescend = true;
				break;
			case 'w':
				GetUserInput().mPressedForward = true;
				break;
			case 's':
				GetUserInput().mPressedBackward = true;
				break;
			case 'a':
				GetUserInput().mPressedLeft = true;
				break;
			case 'd':
				GetUserInput().mPressedRight = true;
				break;
			default:
				assert(false);
			}
		}


		LRESULT CALLBACK PlatformWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
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
			case WM_CHAR:
				OnPressedKey(wParam);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
				break;
			}

			return 0;
		}
	}
}