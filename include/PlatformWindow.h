#pragma once

#include "Common.h"

namespace sr
{
	namespace platform
	{
		class PlatformWindow
		{
		public:
			PlatformWindow() = delete;
			PlatformWindow(const uint16_t frameWidth, const uint16_t frameHeight
				, const gsl::not_null<gsl::cwzstring> className, const gsl::not_null<HINSTANCE> hInstance) noexcept;
			PlatformWindow(const PlatformWindow&) = delete;
			PlatformWindow(const PlatformWindow&&) = delete;
			PlatformWindow& operator=(const PlatformWindow&) = delete;
			PlatformWindow& operator=(const PlatformWindow&&) = delete;
			~PlatformWindow() = default;

			bool Initialize();
			bool DisplayWindow(const int32_t nCmdShow) const noexcept;

			const HWND& GetHwnd() const noexcept;

		private:
			uint16_t mFrameWidth;
			uint16_t mFrameHeight;
			gsl::not_null<gsl::cwzstring> mClassName;
			HWND mHwnd;
			gsl::not_null<HINSTANCE> mHinstance;
		};
	}
}
