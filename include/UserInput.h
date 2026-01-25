#pragma once

namespace sr
{
	struct UserInput
	{
		bool mPressedAscend = false;
		bool mPressedDescend = false;
		bool mPressedForward = false;
		bool mPressedBackward = false;
		bool mPressedLeft = false;
		bool mPressedRight = false;

		int mCursorDeltaX = 0;
		int mCursorDeltaY = 0;
	};
}