#pragma once

#include "Common.h"

namespace sr
{
	struct UserInput;

	namespace platform
	{
		static constexpr uint16_t minFrameWidth = 800;
		static constexpr uint16_t minFrameHeight = 600;
		static constexpr double aspectRatio = 4.0f / 3;

		static_assert(static_cast<double>(minFrameWidth) / minFrameHeight - aspectRatio <= std::numeric_limits<double>::epsilon()
			, "Aspect ratio should remain 4:3. Did you change min frame width/height?");

		class PlatformWindow
		{
		public:
			PlatformWindow(const uint16_t frameWidth, const uint16_t frameHeight
				, const gsl::not_null<gsl::cwzstring> className, int nCmdShow) noexcept;

			PlatformWindow() = delete;
			PlatformWindow(const PlatformWindow&) = delete;
			PlatformWindow(PlatformWindow&&) = delete;
			PlatformWindow& operator=(const PlatformWindow&) = delete;
			PlatformWindow& operator=(PlatformWindow&&) = delete;
			~PlatformWindow() noexcept;


			void Update() noexcept;

			static void OnPressedKey(WPARAM key) noexcept;
			static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;
			static UserInput& GetUserInput();

			inline uint16_t GetFrameWidth() const noexcept
			{
				return mFrameWidth;
			}

			inline uint16_t GetFrameHeight() const noexcept
			{
				return mFrameHeight;
			}

			inline HWND GetWindowHandle() const noexcept
			{
				return mHwnd;
			}

		private:
			uint16_t mFrameWidth = minFrameWidth;
			uint16_t mFrameHeight = minFrameHeight;
			gsl::not_null<gsl::cwzstring> mClassName;
			HWND mHwnd;
		};
	}
}
