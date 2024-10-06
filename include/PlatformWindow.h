#pragma once

#include <limits>
#include "Common.h"

namespace SR
{
	namespace Platform
	{
		constexpr uint16_t minFrameWidth = 800;
		constexpr uint16_t minFrameHeight = 600;
		constexpr double aspectRatio = 4.0f / 3;

		/*assert(std::abs(static_cast<double>(minFrameWidth) / minFrameHeight - aspectRatio) <= std::numeric_limits<double>::epsilon()
		, "Aspect ratio should remain 4:3. Did you change min frame width/height?");*/

		class PlatformWindow
		{
		public:
			PlatformWindow(uint16_t frameWidth, uint16_t frameHeight, gsl::not_null<gsl::wzstring> className, gsl::not_null<HINSTANCE> hInstance);
			bool Initialize();

		private:
			uint16_t mFrameWidth;
			uint16_t mFrameHeight;
			gsl::not_null<gsl::wzstring> mClassName;
			HWND mHwnd;
			gsl::not_null<HINSTANCE> mHinstance;
		};
	}
}
